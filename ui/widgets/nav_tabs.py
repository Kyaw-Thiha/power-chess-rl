from __future__ import annotations
from dataclasses import dataclass
from typing import Iterable

from textual.widget import Widget
from textual.reactive import reactive
from textual.message import Message
from textual.containers import Vertical
from textual.widgets import Static
from textual.app import ComposeResult


@dataclass(slots=True)
class NavItem:
    key: str
    label: str
    icon: str = " "


class NavTabs(Widget):
    """Right-docked vertical navigation with lazygit vibe."""

    items: reactive[list[NavItem]] = reactive(list)
    active_key: reactive[str] = reactive("hotseat")

    class NavSelected(Message):
        """Emitted when a nav item is selected."""

        def __init__(self, key: str) -> None:
            self.key = key
            super().__init__()

    def __init__(self, items: Iterable[NavItem], active_key: str = "hotseat", id: str | None = None) -> None:
        super().__init__(id=id)
        self.items = list(items)
        self.active_key = active_key

    def compose(self) -> ComposeResult:
        with Vertical(id="nav-container"):
            for it in self.items:
                is_active = it.key == self.active_key
                prefix = "▶" if is_active else "  "
                # Use separate class names instead of a spaced string with hyphen
                classes = "item"
                if is_active:
                    classes += " -active"
                yield Static(f"{prefix} {it.label}", classes=classes, id=f"nav-{it.key}")

    def on_mount(self) -> None:
        for it in self.items:
            node = self.query_one(f"#nav-{it.key}", Static)
            node.can_focus = True

    def _refresh_visuals(self) -> None:
        """Update prefix and active class without remounting."""
        for it in self.items:
            node = self.query_one(f"#nav-{it.key}", Static)
            is_active = it.key == self.active_key
            prefix = "▶" if is_active else "  "
            node.update(f"{prefix} {it.label}")
            node.set_class(is_active, "-active")  # toggle the active class

    def on_click(self, event) -> None:  # type: ignore[override]
        target_id = getattr(event.target, "id", None)
        if not target_id or not target_id.startswith("nav-"):
            return
        key = target_id.replace("nav-", "", 1)
        if key == self.active_key:
            return
        self.active_key = key
        self._refresh_visuals()
        self.post_message(self.NavSelected(key))
