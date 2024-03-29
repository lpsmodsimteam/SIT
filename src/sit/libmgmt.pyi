import os
import subprocess

class Commands:
    @staticmethod
    def sanitize(cmd: subprocess.CompletedProcess[bytes]) -> str: ...
    def run_cmd(
        self, cmd: list[str], stdout: int | None = ...
    ) -> subprocess.CompletedProcess[bytes]: ...
    def __get_sst_config(self) -> str: ...
    def __get_prefix(self, sst_config: str) -> os.PathLike: ...
    def __get_include_dir(self, sst_config: str) -> os.PathLike: ...
    def get_dest_dir(self) -> os.PathLike: ...

class LibraryManager:
    def __init__(self) -> None: ...
    def is_installed(self) -> bool: ...
    def get_dest_dir(self) -> os.PathLike: ...
    def install(self) -> None: ...
