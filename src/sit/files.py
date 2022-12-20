import pathlib


class Paths:
    def __init__(self, hdl_str, module_name, module_dir_path):

        self.hdl_str = hdl_str
        self.module_dir = pathlib.Path(module_dir_path)

        self.template_dir_path = (
            pathlib.Path(__file__).parent / "template" / hdl_str
        )
        self.driver_template_path = self.template_dir_path / "driver"
        self.component_template_path = self.template_dir_path / "comp"

        self.gen_dir_path = pathlib.Path("gen")
        self.driver_path = self.comp_path = self.gen_dir_path / module_name

    def set_template_paths(
        self,
        template_dir_path="",
        driver_template_path="",
        component_template_path="",
    ):

        if template_dir_path:
            self.template_dir_path = pathlib.Path(template_dir_path)

        if driver_template_path:
            self.driver_template_path = pathlib.Path(driver_template_path)

        if component_template_path:
            self.component_template_path = pathlib.Path(component_template_path)

    def set_gen_paths(self, gen_dir_path="", driver_path="", comp_path=""):

        if gen_dir_path:
            self.gen_dir_path = pathlib.Path(gen_dir_path)

        if driver_path:
            self.driver_path = pathlib.Path(driver_path)

        if comp_path:
            self.comp_path = pathlib.Path(comp_path)
