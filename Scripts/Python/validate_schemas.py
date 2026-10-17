#!/usr/bin/env python3
"""Read-only structural audit of all current NG-039 fixtures; not D2 acceptance."""
import json
from pathlib import Path
from jsonschema import Draft202012Validator

BASE = Path(__file__).resolve().parent / "Specs/RESULTS-SPEC-039"


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def reject_constant(value):
    raise ValueError(f"non-finite JSON constant: {value}")


def load(path):
    return json.loads(path.read_text(encoding="utf-8"),
                      object_pairs_hook=unique_object,
                      parse_constant=reject_constant)


def main():
    schemas = sorted((BASE / "schemas").glob("*.json"))
    fixtures = sorted((BASE / "fixtures").glob("*.json"))
    failures = 0
    validators = {}
    if not schemas or not fixtures:
        print("FAIL: missing schemas or fixtures")
        return 1
    for path in schemas:
        try:
            schema = load(path)
            Draft202012Validator.check_schema(schema)
            validators[path.stem] = Draft202012Validator(schema)
            print(f"SCHEMA OK {path.name}")
        except Exception as exc:
            failures += 1
            print(f"SCHEMA FAIL {path.name}: {exc}")
    passed = 0
    for path in fixtures:
        matches = [s.stem for s in schemas if path.stem.startswith(s.stem + "-")]
        try:
            if len(matches) != 1 or matches[0] not in validators:
                raise ValueError("missing, invalid or ambiguous schema")
            data = load(path)
            errors = list(validators[matches[0]].iter_errors(data))
            if errors:
                error = errors[0]
                location = "/".join(map(str, error.absolute_path)) or "$"
                raise ValueError(f"{location}: {error.validator} failed ({len(errors)} errors)")
            passed += 1
            print(f"FIXTURE OK {path.name}")
        except Exception as exc:
            failures += 1
            print(f"FIXTURE FAIL {path.name}: {exc}")
    print(f"SUMMARY schemas={len(schemas)} fixtures={len(fixtures)} "
          f"fixtures_passed={passed} failures={failures}; structural only, no D2 acceptance")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
