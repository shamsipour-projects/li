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

from typing import Union, Collection

import blogger as b
import global_values as gv

FA_IR_PREFIX = gv.PREFIX + "fa_IR/"
H_PREFIX = FA_IR_PREFIX + "h/"
H_P_PREFIX = H_PREFIX + "p/"
H_M_PREFIX = H_PREFIX + "m/"
X_PREFIX = FA_IR_PREFIX + "x/"
L_PREFIX = FA_IR_PREFIX + "l/"


def persian_stringifier(c: Union[None, str, Collection[str]]) -> str:
    """stringify a collection of strings using persian commas if it's not a string already
    if c is None; "-" will be returned"""
    return "-" if c is None else c if isinstance(c, str) else "، ".join(c)



# H (Hardware)
h_p = b.SecSpec(
    name="fa_ir_h_p",
    dst_path="docs/fa_IR/h/p",
    url_prefix=H_P_PREFIX,
    src_path="original_content/fa_IR/h/p",
    dst_template_path="templates/fa_IR/h/p/p_template.html",
    src_template_path="templates/fa_IR/h/p/p_template.md",
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/h/p/p_index_template.html",
    index_title="فهرست تابلوها",
    qrpages_template_path="templates/fa_IR/h/p/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/h/p/qr_pages_triangle_template.html"
)

h_m = b.SecSpec(
    name="fa_ir_h_m",
    dst_path="docs/fa_IR/h/m",
    url_prefix=H_M_PREFIX,
    src_path="original_content/fa_IR/h/m",
    dst_template_path="templates/fa_IR/h/m/m_template.html",
    src_template_path="templates/fa_IR/h/m/m_template.md",
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/h/m/m_index_template.html",
    index_title="فهرست ماژول‌ها",
    qrpages_template_path="templates/fa_IR/h/m/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/h/m/qr_pages_triangle_template.html"
)


h_sec = b.SecSpec(
    name="fa_ir_h",
    dst_path="docs/fa_IR/h",
    url_prefix=H_PREFIX,
    src_path="original_content/fa_IR/h",
    sub_secs=[h_p, h_m],
    generate_index=False,
    generate_qr=False,
    generate_qrpages=False,
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
    url_prefix=X_PREFIX,
    src_path="original_content/fa_IR/x",
    dst_template_path="templates/fa_IR/x/x_template.html",
    src_template_path="templates/fa_IR/x/x_template.md",
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/x/x_index_template.html",
    index_title="فهرست آزمایش‌ها",
    qrpages_template_path="templates/fa_IR/x/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/x/qr_pages_triangle_template.html"
)


# L (Learn)


l_sec = b.SecSpec(
    name="fa_ir_l",
    dst_path="docs/fa_IR/l",
    url_prefix=L_PREFIX,
    src_path="original_content/fa_IR/l",
    dst_template_path="templates/fa_IR/l/l_template.html",
    src_template_path="templates/fa_IR/l/l_template.md",
    rules=b.Rules(
        copy_selected_data=True,
        recursive_copy=True,
        overwrite_when_copying=True,
    ),
    index_template_path="templates/fa_IR/l/l_index_template.html",
    index_title="فهرست منابع یادگیری",
    qrpages_template_path="templates/fa_IR/l/qr_pages_table_template.html",
    # qrpages_template_path="templates/fa_IR/l/qr_pages_triangle_template.html"
)


root = b.SecSpec(
    name="fa_ir",
    dst_path="docs/fa_IR",
    url_prefix=FA_IR_PREFIX,
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
