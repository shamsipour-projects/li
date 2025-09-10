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

PREFIX = c.FA_IR_PREFIX + "l/"


@frozen
class ResourceTable:
    name: str = ""
    manufacturing_date: Union[int, str] = ""
    category: str = ""
    manufacturer_name: str = ""
    manufacturer_country: str = ""


@frozen
class ResourceData:
    title: Optional[str] = None
    header: Optional[str] = None
    pic: Optional[str] = None
    table: Optional[ResourceTable] = None
    explanation_paragraphs: Optional[Union[str, Collection[str]]] = None


@frozen
class ResourcesIndexRow:
    filename: str = ""
    link: str = ""
    part_title: str = ""  # There is also a title on top of the page
    # pic: str = ""
    table: Optional[ResourceTable] = None


def md_table_extractor(loaded_file: fm.Post) -> ResourceTable:
    return ResourceTable(
        name=loaded_file.get("name"),
        born=c.persian_stringifier(loaded_file.get("born")),
        died=c.persian_stringifier(loaded_file.get("died")),
        gender=loaded_file.get("gender"),
        nationality=c.persian_stringifier(loaded_file.get("nationality")),
        alma_mater=c.persian_stringifier(loaded_file.get("alma_mater")),
        known_for=c.persian_stringifier(loaded_file.get("known_for")),
        awards=c.persian_stringifier(loaded_file.get("awards")),
        tags=c.persian_stringifier(loaded_file.get("tags"))
    )


def md_data_extractor(dirpath, f) -> ResourceData:
    fl = fm.load(osp.join(dirpath, f))
    return ResourceData(
        title=fl.get("title"),
        header=fl.get("header"),
        pic=fl.get("pic"),
        table=md_table_extractor(fl),
        bio=fl.content
    )


# Index


def soup_table_extractor(soup: BeautifulSoup) -> ResourceTable:
    rows = [row.text.strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    return ResourceTable(
        name=rows[0][1],
        manufacturing_date=rows[0][3],
        category=rows[1][1],
        manufacturer_name=rows[1][3],
        manufacturer_country=rows[2][1]
    )


def escapeless_soup_table_extractor(soup: BeautifulSoup) -> ResourceTable:
    rows = [str(row).strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    # print(rows)
    return ResourceTable(
        name=rows[0][2][9:-5],  # removing "</b><br/>" from start and "</td>" from end
        manufacturing_date=rows[0][4][9:-5],
        category=rows[1][2][9:-5],
        manufacturer_name=rows[1][4][9:-5],
        manufacturer_country=rows[2][2][9:-5]
    )


def index_row_extractor(dirpath: str, f: str) -> ResourcesIndexRow:
    path = osp.join(dirpath, f)
    f_text = b.file_reader(path)
    soup = BeautifulSoup(f_text, "html.parser")
    return ResourcesIndexRow(
        filename=f,
        link=f,
        part_title=soup.title.text,  # TODO
        #table=soup_table_extractor(soup)  # TODO
    )


# Reverse


def html_data_extractor(dirpath: str, f: str, markdownify: bool = False) -> ResourceData:
    path = osp.join(dirpath, f)
    f_text = b.file_reader(path)
    soup = BeautifulSoup(f_text, "html.parser")
    # ep = [p.text for p in e.find_all("p") for e in soup.body.find_all("div", {"class": "main-text"})]
    # ep = []
    # for e in soup.body.find_all("div", {"class": "main-text"}):
    #     for p in e.find_all("p"):
    #         # print(md(str(p)))
    #         ep.append(md(str(p)) if markdownify else p.text)
    return ResourceData(
        title=str(soup.title)[7:-8],
        header=str(soup.h1)[43:-5],  # TODO
        pic=soup.img["src"],
        table=escapeless_soup_table_extractor(soup),
        explanation_paragraphs=str(
            soup.body.find("div", {"class": "main-text"})
        ).lstrip('<div class="main-text">').rstrip('</div>')
    )
