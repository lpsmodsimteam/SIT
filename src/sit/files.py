import pathlib


from .exceptions import TemplateFileNotFound


class Paths:
    def __init__(self, hdl_str, module_dir_path):

        self.hdl_str = hdl_str

        self.__module_dir = pathlib.Path(module_dir_path)

        self.__template_paths = {}
        self.__template_paths["dir"] = (
            pathlib.Path(__file__).parent / "templates" / hdl_str
        ).resolve()
        self.__template_paths["driver"] = (
            self.__template_paths["dir"] / "driver"
        )
        self.__template_paths["comp"] = self.__template_paths["dir"] / "comp"

        self.__gen_paths = {}
        self.__gen_paths["dir"] = pathlib.Path("gen").resolve()

    def get_module_dir(self):

        return self.__module_dir

    def get_template(self, key):

        return self.__template_paths[key]

    def get_gen(self, key):

        return self.__gen_paths[key]

    def get_paths(self):

        return self.__module_dir, self.__template_paths, self.__gen_paths

    def set_template_paths(self, dir="", driver="", comp=""):

        if dir:
            self.__template_paths["dir"] = pathlib.Path(dir)

        if driver:
            self.__template_paths["driver"] = pathlib.Path(driver)

        if comp:
            self.__template_paths["comp"] = pathlib.Path(comp)

    def set_gen_paths(self, dir="", driver="", comp=""):

        if dir:
            self.__gen_paths["dir"] = pathlib.Path(dir)

        if driver:
            self.__gen_paths["driver"] = pathlib.Path(driver)

        if comp:
            self.__gen_paths["comp"] = pathlib.Path(comp)

    def set_driver_path(self, driver_file):

        self.__gen_paths["driver"] = self.__gen_paths["dir"] / driver_file

    def set_comp_path(self, comp_file):

        self.__gen_paths["comp"] = self.__gen_paths["dir"] / comp_file

    def set_extra_file_paths(self, templates, gens=None):

        if gens and templates.keys() == gens.keys():

            for file_key in templates:
                self.__gen_paths[file_key] = (
                    self.__gen_paths["dir"] / gens[file_key]
                )
                self.__template_paths[file_key] = (
                    self.__template_paths["dir"] / templates[file_key]
                )

        else:
            for file_key in templates:
                self.__gen_paths[file_key] = (
                    self.__gen_paths["dir"] / templates[file_key]
                )
                self.__template_paths[file_key] = (
                    self.__template_paths["dir"] / templates[file_key]
                )

    def read_template_str(self, file):

        template_file = self.__template_paths[file]
        if template_file.exists():
            with open(template_file) as fp:
                return fp.read()

        raise TemplateFileNotFound(
            f"Component boilerplate template file: '{template_file}' not found"
        )
