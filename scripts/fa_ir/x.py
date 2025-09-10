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

from os import path as osp
from typing import Collection, Optional, Union

from attrs import frozen
from bs4 import BeautifulSoup
import frontmatter as fm

import blogger as b
from . import common as c

PREFIX = c.FA_IR_PREFIX + "x/"


@frozen
class ExperimentTable:
    name: Optional[str] = None
    # birth_date: Optional[date] = None
    # birth_location: Optional[str] = None
    born: Optional[str] = None
    # death_date: Optional[date] = None
    # death_location: Optional[str] = None
    died: Optional[str] = None
    gender: Optional[bool] = None
    nationality: Optional[Union[str, Collection[str]]] = None  # They can have multiple citizenships
    alma_mater: Optional[Union[str, Collection[str]]] = None
    known_for: Optional[Union[str, Collection[str]]] = None
    awards: Optional[Union[str, Collection[str]]] = None
    tags: Optional[Union[str, Collection[str]]] = None


@frozen
class ExperimentData:
    title: Optional[str] = None
    header: Optional[str] = None
    pic: Optional[str] = None
    table: Optional[ExperimentTable] = None
    bio_summary: Optional[Union[str, Collection[str]]] = None
    bio: Optional[Union[str, Collection[str]]] = None


@frozen
class ExperimentsIndexRow:
    filename: str = ""
    link: str = ""
    scientist_title: str = ""  # There is also a title on top of the page
    # pic: str = ""
    table: Optional[ExperimentTable] = None


def md_table_extractor(loaded_file: fm.Post) -> ExperimentTable:
    return ExperimentTable(
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


def md_data_extractor(dirpath, f) -> ExperimentData:
    fl = fm.load(osp.join(dirpath, f))
    return ExperimentData(
        title=fl.get("title"),
        header=fl.get("header"),
        pic=fl.get("pic"),
        table=md_table_extractor(fl),
        bio=fl.content
    )


# Index


def soup_table_extractor(soup: BeautifulSoup) -> ExperimentTable:
    rows = [row.text.strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    return ExperimentTable(
        name=rows[0][1],
        born=rows[0][3],
        died=rows[1][1],
        gender=True if rows[1][3] == "مرد" else False,
        nationality=rows[2][1],
        alma_mater=rows[2][3],
        known_for=rows[3][1],
        awards=rows[3][3],
        tags=rows[4][1]
    )


def escapeless_soup_table_extractor(soup: BeautifulSoup) -> ExperimentTable:
    rows = [str(row).strip("\n").replace("\n", ":").split(":")
            for row in soup.find_all("tr")]
    return ExperimentTable(
        name=rows[0][2][9:-5],  # removing "</b><br/>" from start and "</td>" from end
        born=rows[0][4][9:-5],
        died=rows[1][2][9:-5],
        gender=True if rows[1][4][9:-5] == "مرد" else False,
        nationality=rows[2][2][9:-5],
        alma_mater=rows[2][4][9:-5],
        known_for=rows[3][2][9:-5],
        awards=rows[3][4][9:-5],
        tags=rows[4][2][9:-5]
    )


def index_row_extractor(dirpath: str, f: str) -> ExperimentsIndexRow:
    path = osp.join(dirpath, f)
    f_text = b.file_reader(path)
    soup = BeautifulSoup(f_text, "html.parser")
    return ExperimentsIndexRow(
        filename=f,
        link=f,
        scientist_title=soup.title.text,  # TODO
        #table=soup_table_extractor(soup)  # TODO
    )


# Reverse


def html_data_extractor(dirpath: str, f: str, markdownify: bool = False) -> ExperimentData:
    path = osp.join(dirpath, f)
    f_text = b.file_reader(path)
    soup = BeautifulSoup(f_text, "html.parser")
    return ExperimentData(
        title=soup.title.text,
        header=soup.h1.text,  # TODO
        pic=soup.img["src"],
        table=escapeless_soup_table_extractor(soup),
        bio=str(
            soup.body.find("div", {"class": "main-text"})
        ).lstrip('<div class="main-text">').rstrip('</div>')
    )
