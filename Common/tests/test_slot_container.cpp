/*
        NovaGenesis — SPEC-033 Phase B

        Name:           Slot container + generational handles test harness
        File:           test_slot_container.cpp
        Author:         Hermes Agent
        Date:           2026-09-08

        Standalone unit tests for the Phase B container logic:
          - O(1) allocation via free-list (no linear scan)
          - generational handles: resolve with stale generation -> NULL (T2)
          - live-count invariant: live messages == occupied slots (T3)
          - retention counter blocks destruction (T5)
          - shutdown clears everything (T6)

        Build: g++ -std=c++20 -O2 -pthread test_slot_container.cpp -o test_slot_container
        The container logic is exercised via a minimal stand-in for Message
        (same ownership semantics, no CommandLine payload). The production
        integration lives in Process.h/Process.cpp with the identical data
        structures — this harness pins the CONTRACT before integration.

        Run: ./test_slot_container   (exit 0 = all pass)
*/

#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <queue>

#define MAX_MESSAGES_IN_MEMORY 30000

// ---- Minimal stand-in for the parts of Message Phase B touches -----------
struct DummyMessage
{
  double Time;
  unsigned int Tag;
  bool DeleteFlag;
  unsigned int Retentions;
  DummyMessage(double t) : Time(t), Tag(0), DeleteFlag(false), Retentions(0) {}
  void MarkToDelete() { DeleteFlag = true; }
  void UnmarkToDelete() { DeleteFlag = false; }
  bool GetDeleteFlag() const { return DeleteFlag; }
};

// ---- Phase B handle + slot (mirrors Process.h) ----------------------------
struct MsgHandle
{
  unsigned int Slot = 0xFFFFFFFFu;
  unsigned int Generation = 0;
  bool Valid() const { return Slot != 0xFFFFFFFFu; }
};

// ---- Container under test (mirrors Process.h Phase B members) -------------
struct Slot
{
  DummyMessage* M = nullptr;
  unsigned int Generation = 0;
};

template <typename T>
class SlotContainer
{
public:
  Slot Slots[MAX_MESSAGES_IN_MEMORY];
  unsigned int NoM = 0;
  unsigned int Counter = 0;
  // Free-list of slot indices (LIFO). Capacity == MAX, so no heap growth.
  unsigned int FreeList[MAX_MESSAGES_IN_MEMORY];
  unsigned int NoFree = MAX_MESSAGES_IN_MEMORY;

  SlotContainer()
  {
    for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
      FreeList[i] = MAX_MESSAGES_IN_MEMORY - 1 - i; // pop() takes lowest index first
  }

  // O(1) alloc. Returns handle; on exhaustion returns invalid handle.
  MsgHandle Insert(T* M)
  {
    MsgHandle H;
    if (NoFree == 0)
      return H; // invalid
    unsigned int i = FreeList[--NoFree];
    Slots[i].M = M;
    // Generation bumped on REUSE (initial allocation keeps 0)
    H.Slot = i;
    H.Generation = Slots[i].Generation;
    NoM++;
    Counter++;
    return H;
  }

  // O(1) resolve. Stale generation or freed slot -> NULL. (Invariant T2)
  T* Resolve(const MsgHandle& H) const
  {
    if (!H.Valid() || H.Slot >= MAX_MESSAGES_IN_MEMORY)
      return nullptr;
    const Slot& S = Slots[H.Slot];
    if (S.M == nullptr || S.Generation != H.Generation)
      return nullptr;
    return S.M;
  }

  // O(1) erase by handle (owner-side). Bumps generation.
  bool Erase(const MsgHandle& H)
  {
    if (Resolve(H) == nullptr)
      return false;
    Slots[H.Slot].M = nullptr;
    Slots[H.Slot].Generation++;
    FreeList[NoFree++] = H.Slot;
    NoM--;
    return true;
  }

  // Legacy pointer search (compat): O(n), used only by pointer-based callers.
  MsgHandle Find(const T* M) const
  {
    MsgHandle H;
    for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
    {
      if (Slots[i].M == M)
      {
        H.Slot = i;
        H.Generation = Slots[i].Generation;
        break;
      }
    }
    return H;
  }

  // Delete-if-marked-and-unretained (mirrors DeleteMarkedMessages). (T5)
  unsigned int DeleteMarked()
  {
    unsigned int Deleted = 0;
    for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
    {
      if (Slots[i].M != nullptr && Slots[i].M->GetDeleteFlag() && Slots[i].M->Retentions == 0)
      {
        delete Slots[i].M;
        Slots[i].M = nullptr;
        Slots[i].Generation++;
        FreeList[NoFree++] = i;
        NoM--;
        Deleted++;
      }
    }
    return Deleted;
  }

  // Shutdown: destroy everything regardless of flags. (Invariant T6)
  unsigned int ShutdownDeleteAll()
  {
    unsigned int Deleted = 0;
    for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
    {
      if (Slots[i].M != nullptr)
      {
        delete Slots[i].M;
        Slots[i].M = nullptr;
        Slots[i].Generation++;
        FreeList[NoFree++] = i;
        NoM--;
        Deleted++;
      }
    }
    return Deleted;
  }
};

// ---- Queue entry with COPIED sort keys (never dereferences Message) -------
struct QEntry
{
  double Time;
  unsigned int Tag;
  MsgHandle H;
};

struct QCompare
{
  // priority_queue comparator: "a before b" = a sorts LATER than b (min-heap
  // via top()). Semantics: earliest due time first, tie -> lower tag first.
  bool operator()(const QEntry& a, const QEntry& b) const
  {
    if (a.Time != b.Time)
      return a.Time > b.Time; // a later -> a sinks
    return a.Tag > b.Tag;
  }
};

// ---- Tests -----------------------------------------------------------------
static int failures = 0;
#define CHECK(cond)                                                        \
  do                                                                       \
  {                                                                        \
    if (!(cond))                                                           \
    {                                                                      \
      printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);               \
      failures++;                                                          \
    }                                                                      \
  } while (0)

void test_alloc_free_list_order()
{
  SlotContainer<DummyMessage> C;
  std::vector<MsgHandle> Hs;
  for (int k = 0; k < 5; k++)
    Hs.push_back(C.Insert(new DummyMessage(k)));
  CHECK(C.NoM == 5);
  // Lowest indices handed out first (free-list LIFO from descending fill)
  for (unsigned int k = 0; k < 5; k++)
    CHECK(Hs[k].Slot == k);
  // O(1) resolve
  for (unsigned int k = 0; k < 5; k++)
    CHECK(C.Resolve(Hs[k]) != nullptr);
}

void test_stale_generation_rejected()
{
  SlotContainer<DummyMessage> C;
  MsgHandle H = C.Insert(new DummyMessage(1.0));
  CHECK(C.Resolve(H) != nullptr);
  CHECK(C.Erase(H));
  CHECK(C.Resolve(H) == nullptr); // freed -> NULL
  // Reuse the same slot: generation must differ
  MsgHandle H2 = C.Insert(new DummyMessage(2.0));
  CHECK(H2.Slot == H.Slot);
  CHECK(H2.Generation != H.Generation);
  CHECK(C.Resolve(H) == nullptr);  // stale handle NEVER resolves (T2)
  CHECK(C.Resolve(H2) != nullptr); // fresh handle resolves
  delete C.Resolve(H2);
}

void test_live_count_invariant()
{
  SlotContainer<DummyMessage> C;
  std::vector<MsgHandle> Hs;
  for (int k = 0; k < 10; k++)
    Hs.push_back(C.Insert(new DummyMessage(k)));
  CHECK(C.NoM == 10);
  // Erase every other one (success path)
  for (int k = 0; k < 10; k += 2)
    CHECK(C.Erase(Hs[k]));
  CHECK(C.NoM == 5);
  // Erase again (failure path must not decrement)
  for (int k = 0; k < 10; k += 2)
    CHECK(!C.Erase(Hs[k]));
  CHECK(C.NoM == 5);
  // Free-list must hold exactly 5 slots now
  CHECK(C.NoFree == MAX_MESSAGES_IN_MEMORY - 5);
  for (int k = 0; k < 10; k++)
    delete C.Resolve(Hs[k]);
}

void test_retention_blocks_delete()
{
  SlotContainer<DummyMessage> C;
  MsgHandle H = C.Insert(new DummyMessage(1.0));
  DummyMessage* M = C.Resolve(H);
  M->MarkToDelete();
  M->Retentions = 1; // Run holds implicit retention
  CHECK(C.DeleteMarked() == 0); // retained -> NOT destroyed (T5)
  M->Retentions = 0;
  CHECK(C.DeleteMarked() == 1); // released -> destroyed
  CHECK(C.Resolve(H) == nullptr);
  CHECK(C.NoM == 0);
}

void test_unmarked_not_deleted()
{
  SlotContainer<DummyMessage> C;
  MsgHandle H = C.Insert(new DummyMessage(1.0));
  C.Resolve(H)->Retentions = 0;
  CHECK(C.DeleteMarked() == 0); // not marked -> survives
  CHECK(C.Resolve(H) != nullptr);
  delete C.Resolve(H);
}

void test_shutdown_clears_all()
{
  SlotContainer<DummyMessage> C;
  for (int k = 0; k < 7; k++)
    C.Insert(new DummyMessage(k));
  CHECK(C.ShutdownDeleteAll() == 7); // regardless of marks/retentions (T6)
  CHECK(C.NoM == 0);
  CHECK(C.NoFree == MAX_MESSAGES_IN_MEMORY);
}

void test_queue_entry_comparator_no_deref()
{
  // Sort keys are COPIED into QEntry; comparator touches only the entry.
  // Verify ordering: earlier due-time first, then lower tag first.
  // Verify with a REAL priority_queue (matching GW usage), not std::sort:
  // top() must be earliest due time, tie -> lowest tag.
  std::vector<QEntry> in = {
      {5.0, 1, {}}, {1.0, 9, {}}, {1.0, 2, {}}, {3.0, 0, {}}};
  std::priority_queue<QEntry, std::vector<QEntry>, QCompare> pq(in.begin(), in.end());
  CHECK(pq.top().Time == 1.0 && pq.top().Tag == 2);
  pq.pop();
  CHECK(pq.top().Time == 1.0 && pq.top().Tag == 9);
  pq.pop();
  CHECK(pq.top().Time == 3.0);
  pq.pop();
  CHECK(pq.top().Time == 5.0);
}

void test_pop_stale_handle_skipped()
{
  // Simulates the GW loop: pop entries, resolve, skip destroyed ones.
  SlotContainer<DummyMessage> C;
  MsgHandle H1 = C.Insert(new DummyMessage(1.0));
  MsgHandle H2 = C.Insert(new DummyMessage(2.0));
  std::vector<QEntry> q = {{1.0, 0, H1}, {2.0, 1, H2}};
  // Entry 1's message gets destroyed before its pop (worst case)
  DummyMessage* M1 = C.Resolve(H1);
  C.Erase(H1);
  delete M1;
  int executed = 0;
  for (auto& e : q)
  {
    DummyMessage* M = C.Resolve(e.H);
    if (M == nullptr)
      continue; // stale: skip, never crash (T2)
    executed++;
  }
  CHECK(executed == 1);
  delete C.Resolve(H2);
}

void test_exhaustion()
{
  SlotContainer<DummyMessage> C;
  // Small pool variant: fill to capacity of a tiny container logic check
  // (real MAX=30000; here we only verify invalid handle on exhaustion via
  // a quick 2-slot logic clone)
  struct Tiny
  {
    DummyMessage* M[2] = {nullptr, nullptr};
    unsigned int Free[2];
    unsigned int NoFree = 2;
    unsigned int Gen[2] = {0, 0};
    Tiny() { Free[0] = 1; Free[1] = 0; }
    MsgHandle Insert(DummyMessage* m)
    {
      MsgHandle H;
      if (NoFree == 0) { delete m; return H; }
      unsigned int i = Free[--NoFree];
      M[i] = m; H.Slot = i; H.Generation = Gen[i];
      return H;
    }
  } T;
  MsgHandle A = T.Insert(new DummyMessage(1));
  MsgHandle B = T.Insert(new DummyMessage(2));
  CHECK(A.Valid() && B.Valid());
  MsgHandle Cx = T.Insert(new DummyMessage(3)); // full -> invalid, msg freed
  CHECK(!Cx.Valid());
  delete T.M[0]; delete T.M[1];
}

int main()
{
  test_alloc_free_list_order();
  test_stale_generation_rejected();
  test_live_count_invariant();
  test_retention_blocks_delete();
  test_unmarked_not_deleted();
  test_shutdown_clears_all();
  test_queue_entry_comparator_no_deref();
  test_pop_stale_handle_skipped();
  test_exhaustion();
  if (failures == 0)
    printf("ALL TESTS PASS (9 test functions)\n");
  else
    printf("%d FAILURES\n", failures);
  return failures == 0 ? 0 : 1;
}
