import pathlib
import subprocess

class Commands:
    @staticmethod
    def sanitize(cmd: subprocess.CompletedProcess[bytes]) -> str: ...
    def run_cmd(self, cmd: list[str]) -> str: ...
    def __get_sst_config(self) -> str: ...
    def __get_prefix(self, sst_config: str) -> pathlib.Path: ...
    def __get_include_dir(self, sst_config: str) -> pathlib.Path: ...
    def get_dest_dir(self) -> pathlib.Path: ...

class LibraryManager:
    def __init__(self) -> None: ...
    def is_installed(self) -> bool: ...
    def install(self) -> None: ...
