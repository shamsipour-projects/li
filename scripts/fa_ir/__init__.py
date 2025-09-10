# Copyright 2023 MohammadMohsen Akbarpoor Darabi (M. MAD)

# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.

from typing import List, Union
import os.path as osp

from attrs import asdict
from jinja2 import Template

import blogger as b
from . import common as c
from . import h
from . import x
from . import l


# Remainant of 'generate_beautiful_qr_codes' (qr codes with src file headers
# as each qr name, instead of qr file basename)
# Could not merge this into the 'qr_pages_extractor', because the qr file
# basename is still needed to link to the actual qr code png file
# def custom_qr_table_writer(sec: b.SecSpec, table: List[List[str]], template: Template,
#                            path: str, mode: str = "w", title: str = "QR Codes"):
#     with open(path, mode=mode) as f:
#         f.write(
#             template.render(
#                 title=title,
#                 table=[
#                     table,
#                     [
#                         sec.custom_data_extractor(
#                             osp.join(sec.src_path, p + ".md")
#                         ).header
#                         for p in table[0]
#                     ]
#                 ],
#                 enumerate=enumerate,
#                 len=len
#             )
#         )
# def custom_qr_table_writer(sec: b.SecSpec, table: List[List[str]], template: Template,
#                            path: str, mode: str = "w", title: str = "QR Codes"):
#     with open(path, mode=mode) as f:
#         f.write(
#             template.render(
#                 title=title,
#                 table=[
#                     table,
#                     [
#                         b.data_extractor(
#                             sec,
#                             osp.join(sec.src_path, p + ".md")
#                         ).get("header")
#                         for p in table[0]
#                     ]
#                 ],
#                 enumerate=enumerate,
#                 len=len
#             )
#         )


# Reverse


def md_data_writer(pd: Union[x.ExperimentData, h.PanelData],
                   template: Template, path: str, mode: str = "w"):
    with open(path, mode) as f:
        f.write(template.render(asdict(pd)))


# H (Hardware)
h_p = b.SecSpec(
    name="fa_ir_h_p",
    dst_path="docs/fa_IR/h/p",
    url_prefix=h.PREFIX + "p/",
    src_path="original_content/fa_IR/h/p",
    data_spec=h.PanelData,
    dst_template_path="templates/fa_IR/h/p/p_template.html",
    src_template_path="templates/fa_IR/h/p/p_template.md",
    #custom_data_extractor=h.md_data_extractor,  # TODO
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/h/p/p_index_template.html",
    #custom_index_extractor=h.index_row_extractor,  # TODO
    index_title="فهرست تابلوها",
    qrpages_template_path="templates/fa_IR/h/p/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/h/p/qr_pages_triangle_template.html",
    #custom_qr_table_writer=custom_qr_table_writer  # TODO
)

h_m = b.SecSpec(
    name="fa_ir_h_m",
    dst_path="docs/fa_IR/h/m",
    url_prefix=h.PREFIX + "m/",
    src_path="original_content/fa_IR/h/m",
    data_spec=h.PanelData,
    dst_template_path="templates/fa_IR/h/m/m_template.html",
    src_template_path="templates/fa_IR/h/m/m_template.md",
    #custom_data_extractor=h.md_data_extractor,  # TODO
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/h/m/m_index_template.html",
    #custom_index_extractor=h.index_row_extractor,  # TODO
    index_title="فهرست ماژول‌ها",
    qrpages_template_path="templates/fa_IR/h/m/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/h/m/qr_pages_triangle_template.html",
    #custom_qr_table_writer=custom_qr_table_writer  # TODO
)


h_sec = b.SecSpec(
    name="fa_ir_h",
    dst_path="docs/fa_IR/h",
    url_prefix=h.PREFIX,
    src_path="original_content/fa_IR/h",
    sub_secs=[h_p, h_m],
    generate_index=False,
    generate_qr=False,
    generate_qrpages=False,
    custom_data_extractor=h.md_data_extractor,
    rules=b.Rules(
        recursive_convert=False,
        copy_selected_data=True,
        recursive_copy=False,
        overwrite_when_copying=True,
    ),
)


# X (EXperiments)


x_sec = b.SecSpec(
    name="fa_ir_x",
    dst_path="docs/fa_IR/x",
    url_prefix=x.PREFIX,
    src_path="original_content/fa_IR/x",
    data_spec=x.ExperimentData,
    dst_template_path="templates/fa_IR/x/x_template.html",
    src_template_path="templates/fa_IR/x/x_template.md",
    #custom_data_extractor=x.md_data_extractor,  # TODO
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/x/x_index_template.html",
    #custom_index_extractor=x.index_row_extractor,  # TODO
    index_title="فهرست آزمایش‌ها",
    qrpages_template_path="templates/fa_IR/x/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/x/qr_pages_triangle_template.html",
    #custom_qr_table_writer=custom_qr_table_writer  # TODO
)


# L (Learn)


l_sec = b.SecSpec(
    name="fa_ir_l",
    dst_path="docs/fa_IR/l",
    url_prefix=l.PREFIX,
    src_path="original_content/fa_IR/l",
    data_spec=l.ResourceData,
    dst_template_path="templates/fa_IR/l/l_template.html",
    src_template_path="templates/fa_IR/l/l_template.md",
    #custom_data_extractor=l.md_data_extractor,  # TODO
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/l/l_index_template.html",
    #custom_index_extractor=x.index_row_extractor,  # TODO
    index_title="فهرست منابع یادگیری",
    qrpages_template_path="templates/fa_IR/l/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/l/qr_pages_triangle_template.html",
    #custom_qr_table_writer=custom_qr_table_writer  # TODO
)


root = b.SecSpec(
    name="fa_ir",
    dst_path="docs/fa_IR",
    url_prefix=c.FA_IR_PREFIX,
    src_path="original_content/fa_IR",
    sub_secs=[h_sec, x_sec, l_sec],
    generate_index=False,
    generate_qr=False,
    generate_qrpages=False,
    rules=b.Rules(
        recursive_convert=False,
        copy_selected_data=True,
        recursive_copy=True,
        copy_selectors=(
            b.MATCH_HTML,
            b.MATCH_CSS,
            b.MATCH_TTF,
            b.MATCH_WOFF,
            b.MATCH_WOFF2
        ),
        overwrite_when_copying=True,
    )
)
