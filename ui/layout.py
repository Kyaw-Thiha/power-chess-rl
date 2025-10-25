from __future__ import annotations
from typing import Iterable, Optional

from textual.app import ComposeResult
from textual.widget import Widget
from textual.containers import Horizontal, Vertical
from textual.widgets import Static

from ui.widgets.nav_tabs import NavTabs, NavItem


class FixedLayout(Widget):
    """
    Lazygit-style fixed layout:
      - Top: TitleBar
      - Left:  Main panel (contains inner #main where pages mount)
      - Right: Vertical NavTabs (fixed width)
      - Bottom: Optional control bar (dock: bottom via CSS)
    """

    def __init__(
        self,
        items: Iterable[NavItem],
        active_key: str = "hotseat",
        control_bar: Optional[Widget] = None,
        id: str | None = None,
    ) -> None:
        super().__init__(id=id)
        self._items = list(items)
        self._active_key = active_key
        self._control_bar = control_bar

    def compose(self) -> ComposeResult:
        # Title bar
        yield Static(" PowerChess RL ", id="titlebar")

        # Main row: left main panel + right nav
        yield Horizontal(
            Vertical(
                Vertical(id="main", classes="main-body"),
                id="main-panel",
            ),
            NavTabs(items=self._items, active_key=self._active_key, id="nav"),
            id="root-row",
        )

        if self._control_bar is not None:
            yield self._control_bar
