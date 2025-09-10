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

import os
from os import path as osp
# from datetime import date
from typing import Optional, Union, Collection

from attrs import asdict, frozen
from bs4 import BeautifulSoup
import frontmatter as fm

import blogger as b
from . import common as c

PREFIX = c.FA_IR_PREFIX + "h/"


@frozen
class PanelTable:
    name: str = ""
    manufacturing_date: Union[int, str] = ""
    category: str = ""
    manufacturer_name: str = ""
    manufacturer_country: str = ""


@frozen
class PanelData:
    title: Optional[str] = None
    header: Optional[str] = None
    pic: Optional[str] = None
    table: Optional[PanelTable] = None
    explanation_paragraphs: Optional[Union[str, Collection[str]]] = None


@frozen
class PanelsIndexRow:
    filename: str = ""
    link: str = ""
    part_title: str = ""  # There is also a title on top of the page
    # pic: str = ""
    table: Optional[PanelTable] = None


def md_table_extractor(loaded_file: fm.Post) -> PanelTable:
    return PanelTable(
        name=loaded_file["name"],
        manufacturing_date=loaded_file["manufacturing_date"],
        category=loaded_file["category"],
        manufacturer_name=loaded_file["manufacturer_name"],
        manufacturer_country=loaded_file["manufacturer_country"]
    )


def md_data_extractor(path: str) -> PanelData:
    fl = fm.load(path)
    # print("[debug]", asdict(fl))
    return PanelData(
        title=fl["title"],
        header=fl["header"],
        pic=fl["pic"],
        table=md_table_extractor(fl),
        explanation_paragraphs=fl.content
    )


# Index


def soup_table_extractor(soup: BeautifulSoup) -> PanelTable:
    rows = [row.text.strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    return PanelTable(
        name=rows[0][1],
        manufacturing_date=rows[0][3],
        category=rows[1][1],
        manufacturer_name=rows[1][3],
        manufacturer_country=rows[2][1]
    )


def escapeless_soup_table_extractor(soup: BeautifulSoup) -> PanelTable:
    rows = [str(row).strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    # print(rows)
    return PanelTable(
        name=rows[0][2][9:-5],  # removing "</b><br/>" from start and "</td>" from end
        manufacturing_date=rows[0][4][9:-5],
        category=rows[1][2][9:-5],
        manufacturer_name=rows[1][4][9:-5],
        manufacturer_country=rows[2][2][9:-5]
    )


def index_row_extractor(dst: str) -> PanelsIndexRow:
    dirpath, f = osp.split(dst)
    f_text = b.file_reader(dst)
    soup = BeautifulSoup(f_text, "html.parser")
    return PanelsIndexRow(
        filename=f,
        link=f,
        part_title=soup.title.text,
        table=soup_table_extractor(soup)
    )


# Reverse


def html_data_extractor(dirpath: str, f: str, markdownify: bool = False) -> PanelData:
    path = osp.join(dirpath, f)
    f_text = b.file_reader(path)
    soup = BeautifulSoup(f_text, "html.parser")
    # ep = [p.text for p in e.find_all("p") for e in soup.body.find_all("div", {"class": "main-text"})]
    # ep = []
    # for e in soup.body.find_all("div", {"class": "main-text"}):
    #     for p in e.find_all("p"):
    #         # print(md(str(p)))
    #         ep.append(md(str(p)) if markdownify else p.text)
    return PanelData(
        title=str(soup.title)[7:-8],
        header=str(soup.h1)[43:-5],  # TODO
        pic=soup.img["src"],
        table=escapeless_soup_table_extractor(soup),
        explanation_paragraphs=str(
            soup.body.find("div", {"class": "main-text"})
        ).lstrip('<div class="main-text">').rstrip('</div>')
    )
