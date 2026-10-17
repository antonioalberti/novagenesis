#!/usr/bin/env python3
"""Synthetic structural mutations only; never a runtime/acceptance validator."""
import copy
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from validate_schemas import load, unique_object, reject_constant
from jsonschema import Draft202012Validator

BASE = ROOT / 'Specs/RESULTS-SPEC-039'

class EvidenceSchemas(unittest.TestCase):
    def validator(self, stem):
        schema = load(BASE / 'schemas' / (stem + '.json'))
        Draft202012Validator.check_schema(schema)
        return Draft202012Validator(schema)

    def run_record(self, suffix='status-pass'):
        return load(BASE / 'fixtures' / ('ng039-d2-run-record-v2-' + suffix + '.json'))

    def reject(self, data):
        self.assertFalse(self.validator('ng039-d2-run-record-v2').is_valid(data))

    def test_all_16_positive_shapes(self):
        paths = sorted((BASE / 'fixtures').glob('*.json'))
        self.assertEqual(len(paths), 16)
        for path in paths:
            with self.subTest(fixture=path.name):
                stems = [p.stem for p in (BASE / 'schemas').glob('*.json') if path.stem.startswith(p.stem + '-')]
                self.assertEqual(len(stems), 1)
                self.validator(stems[0]).validate(load(path))

    def test_pass_mutations(self):
        mutations = [('record_finalized', False), ('failure', self.run_record('status-fail')['failure'])]
        for field, value in mutations:
            with self.subTest(field=field):
                data = self.run_record(); data[field] = value; self.reject(data)
        for section, field, value in [
            ('exit', 'harness_code', None), ('exit', 'timeout_fired', True),
            ('assertions', 'q1_admission', 'FAIL'), ('assertions', 'q0_admission', 'NOT_RUN'),
            ('cleanup', 'complete', False), ('observations', 'counter_overflow', True),
            ('observations', 'q1', None), ('reservations', 'exclusive_reservation_complete', False)]:
            with self.subTest(section=section, field=field):
                data = self.run_record(); data[section][field] = value; self.reject(data)
        data = self.run_record(); data['artifacts'].pop(); self.reject(data)

    def test_fail_mutations(self):
        for field, value in [('record_finalized', False), ('failure', None)]:
            data = self.run_record('status-fail'); data[field] = value; self.reject(data)
        for value in [124, 127, 255]:
            data = self.run_record('status-fail'); data['exit']['outer_code'] = value; self.reject(data)
        data = self.run_record('status-fail'); data['assertions'] = dict.fromkeys(data['assertions'], 'PASS'); self.reject(data)

    def test_partial_reservation_modes(self):
        pending = self.run_record('partial-reservation-inconclusive')
        self.validator('ng039-d2-run-record-v2').validate(pending)
        final = copy.deepcopy(pending)
        final['record_finalized'] = True
        final['common_authority_validated'] = True
        final['failure'] = {'code': 'PARTIAL_RESERVATION', 'phase': 'reservation',
            'message': 'Synthetic partial reservation', 'last_event_sequence': None,
            'inventory_availability': {'pre': False, 'post': False, 'final': False},
            'cleanup_outcome': 'unknown', 'missing_artifacts': []}
        self.validator('ng039-d2-run-record-v2').validate(final)
        for status in ['PASS', 'FAIL']:
            data = copy.deepcopy(final); data['status'] = status; self.reject(data)
        data = copy.deepcopy(final); data['failure']['cleanup_outcome'] = 'complete'; self.reject(data)
        data = copy.deepcopy(final); data['common_authority_validated'] = False; self.reject(data)
        data = copy.deepcopy(pending); data['failure'] = final['failure']; self.reject(data)
        data = copy.deepcopy(pending); data['observations']['q0'] = self.run_record()['observations']['q0']; self.reject(data)

    def test_invalid_fields(self):
        for field, value in [('unexpected', True), ('spec_sha256', 'not-a-hash'), ('status', 'OK')]:
            data = self.run_record(); data[field] = value; self.reject(data)
        data = self.run_record(); data['observations']['q0']['cycles'] = -1; self.reject(data)
        data = self.run_record(); data['exit']['outer_code'] = True; self.reject(data)
        events = load(BASE / 'fixtures/ng039-observer-events-v2-status-pass.json')
        events[0]['unexpected'] = True
        self.assertFalse(self.validator('ng039-observer-events-v2').is_valid(events))

    def test_strict_json(self):
        for text in ['{"x":1,"x":2}', '{"a":{"x":1,"x":2}}', '{"x":NaN}', '{"x":Infinity}', '{broken']:
            with self.subTest(text=text), self.assertRaises(ValueError):
                json.loads(text, object_pairs_hook=unique_object, parse_constant=reject_constant)

if __name__ == '__main__':
    unittest.main(verbosity=2)
