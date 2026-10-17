#!/usr/bin/env python3
"""Unit tests for the link checker."""
import hashlib
import os
import tempfile
import unittest
from pathlib import Path

from checkers.link_checker import validate_link_file

class TestLinkChecker(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.build_root = Path(self.temp_dir.name)
        # Create a dummy compiler binary (we'll use the real one from the system)
        self.compiler = Path("/usr/bin/x86_64-linux-gnu-g++-13")
        if not self.compiler.exists():
            # Fallback to any g++ for testing
            self.compiler = Path("/usr/bin/g++")
            if not self.compiler.exists():
                self.skipTest("No g++ compiler found")
        self.target = "test_target"
        # Create the expected CMakeFiles directory and link.txt path
        self.cmake_dir = self.build_root / "CMakeFiles" / f"{self.target}.dir"
        self.cmake_dir.mkdir(parents=True)
        self.link_file = self.cmake_dir / "link.txt"
        # Create dummy object files that tests may reference
        (self.build_root / "obj1.o").touch()
        (self.build_root / "obj2.o").touch()

    def tearDown(self):
        self.temp_dir.cleanup()

    def _create_link_command(self, argv):
        content = " ".join(argv)
        self.link_file.write_text(content, encoding="utf-8")
        return str(self.link_file), str(self.build_root), self.target

    def test_valid_diagnostic_link(self):
        # A valid link command for the diagnostic target
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
            "obj2.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        result = validate_link_file(link_file, build_root, target)
        self.assertEqual(result["status"], "PASS")
        self.assertEqual(result["compiler"], str(self.compiler.resolve()))
        # Check the hash
        expected_hash = hashlib.sha256(Path(link_file).read_bytes()).hexdigest()
        self.assertEqual(result["link_file_sha256"], expected_hash)

    def test_missing_output(self):
        argv = [
            str(self.compiler),
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("missing -o flag", str(cm.exception))

    def test_duplicate_output(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-o", "other",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("duplicate -o flag", str(cm.exception))

    def test_missing_wl_bdynamic(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("missing -Wl,-Bdynamic", str(cm.exception))

    def test_missing_lc(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("missing -lc", str(cm.exception))

    def test_static_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-static",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -static", str(cm.exception))

    def test_static_libgcc(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-static-libgcc",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -static-libgcc", str(cm.exception))

    def test_static_libstdcxx(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-static-libstdc++",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -static-libstdc++", str(cm.exception))

    def test_shared_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-shared",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -shared", str(cm.exception))

    def test_dynamic_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-dynamic",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -dynamic", str(cm.exception))

    def test_bdynamic_direct_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Bdynamic",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -Bdynamic", str(cm.exception))

    def test_pie_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-pie",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -pie", str(cm.exception))

    def test_rdynamic_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-rdynamic",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forbidden flag -rdynamic", str(cm.exception))

    def test_wl_with_other_option(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "--as-needed",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("unsupported linker option via -Wl: --as-needed", str(cm.exception))

    def test_wl_multiple_options(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic,--foo",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("unsupported linker option via -Wl: --foo", str(cm.exception))

    def test_xlinker_flag(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Xlinker",
            "--some-option",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("forwarded linker control -Xlinker unsupported", str(cm.exception))

    def test_response_file(self):
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "@response.txt",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("response files unsupported", str(cm.exception))

    def test_input_file_does_not_exist(self):
        # We now require input files to exist, so this test should fail.
        # However, we keep the test to ensure nonexistent input is rejected.
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "nonexistent.o",
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("input file does not exist or is not a regular file", str(cm.exception))

    def test_input_file_outside_build_root(self):
        # Create a file outside the build root
        outside_dir = Path(tempfile.mkdtemp())
        outside = outside_dir / "outside.o"
        outside.write_text("dummy", encoding="utf-8")
        # Print for debugging
        print(f"DEBUG: build_root={self.build_root}")
        print(f"DEBUG: outside={outside}")
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic",
            "-l", "c",
            str(outside),
        ]
        link_file, build_root, target = self._create_link_command(argv)
        with self.assertRaises(ValueError) as cm:
            validate_link_file(link_file, build_root, target)
        self.assertIn("input file escapes build root", str(cm.exception))

    def test_link_file_is_symlink(self):
        # Create a symlink to the expected link file
        symlink = self.build_root / "link.txt.symlink"
        symlink.symlink_to(self.link_file)
        argv = [
            str(self.compiler),
            "-o", "test_target",
            "-Wl", "-Bdynamic",
            "-l", "c",
            "obj1.o",
        ]
        content = " ".join(argv)
        self.link_file.write_text(content, encoding="utf-8")
        with self.assertRaises(ValueError) as cm:
            validate_link_file(str(symlink), str(self.build_root), self.target)
        self.assertIn("link.txt must be a regular non-symlink file", str(cm.exception))

    def test_link_file_not_found(self):
        missing = self.build_root / "missing.txt"
        with self.assertRaises(ValueError) as cm:
            validate_link_file(str(missing), str(self.build_root), self.target)
        self.assertIn("link.txt path is not the expected target file", str(cm.exception))

if __name__ == "__main__":
    unittest.main()