"""Synthetic-only SPEC-039 compile grammar regressions; never run a compiler."""
import os
import json
import shlex
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest

from checkers import compile_checker as checker


class CompileCheckerTests(unittest.TestCase):
    def setUp(self):
        # Fail rather than silently fall back to system /tmp.
        self.temp = tempfile.TemporaryDirectory(dir=os.environ['TMPDIR'])
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / 'source'
        self.build = self.root / 'build'
        self.source.mkdir()
        self.build.mkdir()
        (self.source / 'unit.cpp').write_text('// synthetic\n')

    def database(self, profile='default'):
        result = []
        for target in sorted(checker.EXPECTED[profile]):
            output = f'CMakeFiles/{target}.dir/unit.cpp.o'
            args = [checker.COMPILER, *sorted(checker.REQUIRED), '-std=gnu++20']
            if profile == 'diagnostic':
                args.append('-D' + checker.OBSERVER + '=1')
            args += ['-o', output, '-c', str(self.source / 'unit.cpp')]
            result.append(dict(directory=str(self.build), file=str(self.source / 'unit.cpp'),
                               output=output, arguments=args))
        return result

    def validate(self, db, profile='default'):
        return checker.validate_compile_database(db, profile, str(self.source), str(self.build))

    def test_unknown_driver_flag_rejected(self):
        db = self.database()
        db[0]['arguments'].append('-funknown-ng039')
        with self.assertRaises(ValueError):
            self.validate(db)

    def test_both_profiles(self):
        for profile in checker.EXPECTED:
            with self.subTest(profile=profile):
                self.assertEqual(self.validate(self.database(profile), profile)['entry_count'],
                                 len(checker.EXPECTED[profile]))

    def test_supported_operand_forms(self):
        for profile in checker.EXPECTED:
            for joined in (False, True):
                for switch, value in (('-I', 'include.cpp'), ('-isystem', 'system.cpp'),
                                      ('-iquote', 'quote.cpp'), ('-MF', 'deps.cpp'),
                                      ('-MT', 'target.cpp'), ('-D', 'OTHER(x)=x'),
                                      ('-U', 'OTHER')):
                    with self.subTest(profile=profile, joined=joined, switch=switch):
                        db = self.database(profile)
                        db[0]['arguments'] += [switch + value] if joined else [switch, value]
                        self.validate(db, profile)
                db = self.database(profile)
                for entry in db:
                    args = entry['arguments']
                    i = args.index('-o')
                    if joined:
                        args[i:i+2] = ['-o' + args[i+1]]
                    if profile == 'diagnostic' and not joined:
                        i = args.index('-D' + checker.OBSERVER + '=1')
                        args[i:i+1] = ['-D', checker.OBSERVER + '=1']
                    args += ['-MD', '-MMD', '-MP', '-O0']
                self.validate(db, profile)

    def test_missing_or_flag_operands(self):
        for switch in ('-o', '-MF', '-MT', '-I', '-isystem', '-iquote', '-D', '-U'):
            for tail in ([], [''], ['-Wall'], ['@response'], ['\n']):
                with self.subTest(switch=switch, tail=tail):
                    db = self.database()
                    db[0]['arguments'] += [switch] + tail
                    with self.assertRaises(ValueError):
                        self.validate(db)

    def test_unknown_options(self):
        for args in (['-m32'], ['-fno-exceptions'], ['-ggdb'], ['-O2'], ['-std=c++17'],
                     ['-ansi'], ['-traditional'], ['-x', 'c++'], ['-specs=a'],
                     ['-B/tool'], ['-wrapper', 'tool'], ['-fplugin=a'], ['@response'],
                     ['-Wp,-D' + checker.OBSERVER + '=1'], ['-Xpreprocessor', '-DOTHER'],
                     ['-includea'], ['-include', 'a'], ['-imacrosa'], ['-imacros', 'a'],
                     ['--'], ['opaque'], ['-Wl,-Bdynamic'], ['-static']):
            with self.subTest(args=args):
                db = self.database()
                db[0]['arguments'] += args
                with self.assertRaises(ValueError):
                    self.validate(db)

    def test_observer_macro_bypasses(self):
        for profile in checker.EXPECTED:
            for joined in (False, True):
                for switch, value in (('-D', checker.OBSERVER), ('-D', checker.OBSERVER + '=0'),
                                      ('-D', checker.OBSERVER + '=1'),
                                      ('-D', checker.OBSERVER + '(x)=1'),
                                      ('-U', checker.OBSERVER), ('-U', checker.OBSERVER + '(x)')):
                    with self.subTest(profile=profile, joined=joined, value=value):
                        db = self.database(profile)
                        db[0]['arguments'] += [switch + value] if joined else [switch, value]
                        with self.assertRaises(ValueError):
                            self.validate(db, profile)

    def test_malformed_macros(self):
        for value in ('1BAD=1', 'BAD(', 'BAD(x,,y)=1', 'BAD(x-y)=1', '=1'):
            db = self.database()
            db[0]['arguments'].append('-D' + value)
            with self.subTest(value=value), self.assertRaises(ValueError):
                self.validate(db)

    def test_duplicate_output(self):
        for joined in (False, True):
            db = self.database()
            out = db[0]['output']
            db[0]['arguments'] += ['-o' + out] if joined else ['-o', out]
            with self.subTest(joined=joined), self.assertRaises(ValueError):
                self.validate(db)

    def test_output_escape(self):
        for out in ('../CMakeFiles/Common.dir/unit.o', str(self.root / 'CMakeFiles/Common.dir/unit.o')):
            db = self.database()
            args = db[0]['arguments']
            args[args.index('-o') + 1] = out
            db[0]['output'] = out
            with self.subTest(out=out), self.assertRaises(ValueError):
                self.validate(db)

    def test_symlink_escape(self):
        (self.build / 'escape').symlink_to(self.root, target_is_directory=True)
        db = self.database()
        args = db[0]['arguments']
        args[args.index('-o') + 1] = 'escape/CMakeFiles/Common.dir/unit.o'
        db[0].pop('output')
        with self.assertRaises(ValueError):
            self.validate(db)

    def test_source_and_directory_escape(self):
        outside = self.root / 'outside.cpp'
        outside.write_text('// synthetic\n')
        for field in ('file', 'directory'):
            db = self.database()
            db[0][field] = str(outside if field == 'file' else self.root)
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.validate(db)

    def test_metadata_mismatch(self):
        for field in ('output', 'command', 'file'):
            db = self.database()
            (self.source / 'other.cpp').write_text('// synthetic\n')
            db[0][field] = {'output': 'CMakeFiles/PGCS.dir/wrong.o',
                            'command': shlex.join(db[0]['arguments'] + ['-O0']),
                            'file': str(self.source / 'other.cpp')}[field]
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.validate(db)

    def test_command_encoding(self):
        for both in (False, True):
            db = self.database()
            for entry in db:
                entry['command'] = shlex.join(entry['arguments'])
                if not both:
                    del entry['arguments']
            self.validate(db)

    def test_pins_and_cardinality(self):
        for change in ('compiler', 'census', 'duplicate', 'source', 'standard', 'compile', 'required', 'target'):
            db = self.database()
            args = db[0]['arguments']
            if change == 'compiler':
                args[0] = '/not/the/compiler'
            elif change == 'census':
                db.pop()
            elif change == 'duplicate':
                db.append(dict(db[0]))
            elif change == 'source':
                args.append(str(self.source / 'unit.cpp'))
            elif change == 'standard':
                args.append('-std=gnu++20')
            elif change == 'compile':
                args.remove('-c')
            elif change == 'required':
                args.remove('-g3')
            else:
                args[args.index('-o')+1] = 'CMakeFiles/Unknown.dir/unit.o'
                db[0].pop('output')
            with self.subTest(change=change), self.assertRaises(ValueError):
                self.validate(db)

    def test_cli(self):
        path = self.root / 'compile_commands.json'
        script = str(Path(checker.__file__).resolve())
        for profile in checker.EXPECTED:
            db = self.database(profile)
            path.write_text(json.dumps(db))
            command = [sys.executable, script, str(path), profile, str(self.source), str(self.build)]
            result = subprocess.run(command, capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(json.loads(result.stdout)['profile'], profile)
            db[0]['arguments'].append('-funknown-ng039')
            path.write_text(json.dumps(db))
            self.assertEqual(subprocess.run(command, capture_output=True, check=False).returncode, 2)
        self.assertEqual(subprocess.run([sys.executable, script], capture_output=True,
                                        check=False).returncode, 2)


if __name__ == '__main__':
    unittest.main()
