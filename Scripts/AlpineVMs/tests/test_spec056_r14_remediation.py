"""SPEC-056 R14 bounded protected-input regressions."""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import local_provenance
import ng_remote_executor as executor


R14_CASES = {
    "shell-concatenated.log": (
        'tool --token "R14 shell value with spaces"SUFFIX',
        ("R14 shell value with spaces", "SUFFIX"),
    ),
    "json-repr.log": (
        r'''{"argv": ["--password", "R14-json value\ with\ spaces\;\"quote\"SUFFIX"]}''',
        (r'R14-json value\ with\ spaces\;\"quote\"SUFFIX', "SUFFIX"),
    ),
    "python-repr.log": (
        r'''("--api-key", 'R14-python value with spaces\;\'quote\'SUFFIX')''',
        (r"R14-python value with spaces\;\'quote\'SUFFIX", "SUFFIX"),
    ),
    "flag-value.log": (
        "tool --secret R14-bare-secretSUFFIX",
        ("R14-bare-secretSUFFIX", "SUFFIX"),
    ),
    "flag-equals.log": (
        "tool --credential=R14-equals-secretSUFFIX",
        ("R14-equals-secretSUFFIX", "SUFFIX"),
    ),
    "unterminated.log": (
        'tool --token "R14 unterminated value SUFFIX',
        ("R14 unterminated value SUFFIX", "SUFFIX"),
    ),
    "ambiguous.log": (
        'tool --token "R14 ambiguous"SUFFIX trailing',
        ("R14 ambiguous", "SUFFIX"),
    ),
}


def test_r14_full_serialized_config_has_no_secret_or_suffix_fragments():
    records = {name: text for name, (text, _fragments) in R14_CASES.items()}
    serialized = json.dumps(local_provenance.sanitize_config(records), ensure_ascii=False, sort_keys=True)
    for _name, (_raw, fragments) in R14_CASES.items():
        for fragment in fragments:
            assert fragment not in serialized
    assert "<redacted>" in serialized


def test_r14_evidence_records_are_replaced_and_blocked_without_raw_fragments(tmp_path: Path):
    for name, (text, _fragments) in R14_CASES.items():
        path = tmp_path / name
        path.write_text(text, encoding="utf-8")

    report = local_provenance.sanitize_evidence_tree(tmp_path)
    assert report["ok"] is False
    assert any("protected-input" in blocker for blocker in report["blockers"])
    serialized = "\n".join(
        path.read_text(encoding="utf-8") for path in sorted(tmp_path.rglob("*")) if path.is_file()
    )
    for _name, (_raw, fragments) in R14_CASES.items():
        for fragment in fragments:
            assert fragment not in serialized
    assert all(path.read_text(encoding="utf-8") == "<redacted>" for path in tmp_path.iterdir())


def test_r14_non_secret_argv_controls_remain_unchanged():
    controls = {
        "argv": ["tool", "--verbose", "ordinary-value", "--tokenizer", "safe"],
        "command": 'tool --verbose "ordinary value with spaces"',
    }
    assert local_provenance.sanitize_config(controls) == controls


def test_r14_remote_schema_v1_publication_regression_unchanged(tmp_path: Path):
    (tmp_path / "payload.txt").write_text("payload", encoding="utf-8")
    manifest = executor.write_evidence_manifest(tmp_path, {"mode": "remote", "trial_id": "r14"})
    assert json.loads(manifest.read_text(encoding="utf-8"))["schema_version"] == 1
    assert (tmp_path / "manifest.sha256").is_file()
    assert not (tmp_path / "terminal-seal.json").exists()
