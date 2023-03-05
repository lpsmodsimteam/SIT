class ConfigFile:
    def generate_linkwrapper_binding(
        self,
        module_name: str,
        lib: str,
        input_ports: list[dict[str, str | int]],
    ) -> str: ...
    def generate_comps_func(self, module_name: str) -> str: ...
