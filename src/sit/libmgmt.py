import pathlib
import subprocess


class Commands:
    @staticmethod
    def sanitize(cmd):

        return cmd.stdout.decode("utf-8")[:-1]

    def run_cmd(self, cmd):

        return self.sanitize(subprocess.run(cmd, stdout=subprocess.PIPE))

    def __get_sst_config(self):

        return self.run_cmd(["which", "sst-config"])

    def __get_prefix(self, sst_config):

        return pathlib.Path(self.run_cmd([sst_config, "--prefix"]))

    def __get_include_dir(self, sst_config):

        return pathlib.Path(self.run_cmd([sst_config, "--includedir"]))

    def get_dest_dir(self):

        sst_config = self.__get_sst_config()
        prefix = self.__get_prefix(sst_config)
        include_dir = self.__get_include_dir(sst_config)

        return prefix / include_dir.name / "sst/sit"


class LibraryManager:
    def __init__(self):

        cmd = Commands()
        self.lib_dir = pathlib.Path.cwd() / "src/sit/cpp/"
        self.dest_dir = cmd.get_dest_dir()

    def is_installed(self):

        return self.dest_dir.exists()

    def install(self):

        if self.is_installed():
            print("SIT is already installed on the system")
        else:
            print("mkdir .build && cd .build")
            print(f"cmake {self.lib_dir}")
            print("sudo make install")
