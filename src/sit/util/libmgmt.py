import sysconfig
import pathlib
import subprocess


class _Commands:
    @staticmethod
    def capture_output(cmd):

        return cmd.stdout.decode("utf-8")[:-1]

    def run_cmd(self, cmd, stdout=subprocess.PIPE):

        return subprocess.run(cmd, stdout=stdout)

    def __get_sst_config(self):

        sst_config = self.run_cmd(["which", "sst-config"])
        return self.capture_output(sst_config)

    def __get_prefix(self, sst_config):

        prefix = self.run_cmd([sst_config, "--prefix"])
        return pathlib.Path(self.capture_output(prefix))

    def __get_include_dir(self, sst_config):

        include_dir = self.run_cmd([sst_config, "--includedir"])
        return pathlib.Path(self.capture_output(include_dir))

    def get_dest_dir(self):

        sst_config = self.__get_sst_config()
        prefix = self.__get_prefix(sst_config)
        include_dir = self.__get_include_dir(sst_config)

        return prefix / include_dir.name / "sst/sit"


class LibraryManager:
    def __init__(self):

        self.__cmd = _Commands()
        self.__dest_dir = self.__cmd.get_dest_dir()

    def is_installed(self):

        return self.__dest_dir.exists()

    def get_dest_dir(self):

        return self.__dest_dir

    def install(self):

        lib_dir = pathlib.Path(sysconfig.get_paths()["purelib"]) / "sit/cpp"
        try:
            self.__cmd.run_cmd(["cmake", lib_dir], stdout=None)

        except FileNotFoundError:
            print("CMake not found")
            exit(-1)

        self.__cmd.run_cmd(["sudo", "make", "install"], stdout=None)
