from .render import TemplateRenderer

LINK_WRAPPER_STR = """{{module_name}}_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
{{module_name}}_link->set_din_link(configureLink("{{module_name}}_din"));
{{module_name}}_link->set_dout_link(configureLink(
    "{{module_name}}_dout",
    new SST::Event::Handler<{{lib}}>(this, &{{lib}}::{{module_name}})
));
{{module_name}}_link->set_buffer_lengths({{buf_lens}});
"""

CONNECT_COMPS_FUNC_STR = """
def connect_comps(comp, comp_name, main_comp_name):
    sst.Link(main_comp_name + "_din").connect(
        (comp, comp_name + "_din", LINK_DELAY),
        ({{module_name}}_main, main_comp_name + "_din", LINK_DELAY),
    )
    sst.Link(main_comp_name + "_dout").connect(
        (comp, comp_name + "_dout", LINK_DELAY),
        ({{module_name}}_main, main_comp_name + "_dout", LINK_DELAY),
    )
"""


class ConfigFile:
    def __init__(self) -> None:

        self.template = TemplateRenderer()

    def generate_linkwrapper_binding(self, module_name, lib, buf_lens):

        return self.template.render(
            LINK_WRAPPER_STR,
            {
                "module_name": module_name,
                "lib": lib,
                "buf_lens": buf_lens,
            },
        )

    def generate_comps_func(self, module_name):

        return self.template.render(
            CONNECT_COMPS_FUNC_STR, {"module_name": module_name}
        )
