import os
import sys
import venv
import glob
import tempfile
import unittest
import subprocess

DIR = os.path.realpath(os.path.join(__file__, "..", "..", ".."))


class TestPackage(unittest.TestCase):
    """Test if package can be installed (takes a looong time)."""

    @unittest.skipUnless(
        sys.platform != "win32" and sys.version_info.minor >= 8,
        "pip location on windows",
    )
    def _test_can_install_source_dist(self):
        with tempfile.TemporaryDirectory() as tmpdirname:
            result = subprocess.run(
                [
                    sys.executable,
                    os.path.join(DIR, "setup.py"),
                    "sdist",
                    f"--dist-dir={tmpdirname}",
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )

            self.assertEqual(result.returncode, 0, msg=result.stderr)

            source_dist, *_ = glob.glob(f"{tmpdirname}/*")
            self.assertTrue(os.path.isfile(source_dist))

            venv_dir = os.path.join(tmpdirname, "venv")
            venv.create(venv_dir, with_pip=True)

            subprocess.check_call(
                [os.path.join(venv_dir, "bin", "pip"), "install", source_dist],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )


if __name__ == "__main__":
    unittest.main()
