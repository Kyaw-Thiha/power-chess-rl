from __future__ import annotations
from typing import Iterable, Optional

from textual.app import ComposeResult
from textual.widget import Widget
from textual.containers import Horizontal, Vertical

from ui.widgets.nav_tabs import NavTabs, NavItem


class FixedLayout(Widget):
    """
    Lazygit-style fixed layout:
      - Left:  Main panel with a border title, contains an inner #main where pages mount.
      - Right: Vertical NavTabs with fixed width.
      - Bottom: Optional control bar (dock: bottom via CSS).

    Notes:
      - We keep IDs (#root-row, #main-panel, #main, #nav) so CSS can target reliably.
      - The control bar (if provided) is yielded *before* StatusBar so StatusBar remains the bottom-most.
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
        # Main row: left main panel + right nav
        yield Horizontal(
            Vertical(  # Outer bordered panel with title; inner #main is where pages mount
                Vertical(id="main", classes="main-body"),
                id="main-panel",
            ),
            NavTabs(items=self._items, active_key=self._active_key, id="nav"),
            id="root-row",
        )

        # Optional control bar (dock: bottom via CSS). StatusBar should be yielded by App after this.
        if self._control_bar is not None:
            yield self._control_bar
