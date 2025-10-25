from __future__ import annotations

from textual.app import App, ComposeResult
from textual.reactive import reactive
from textual import on
from textual.widget import Widget

from ui.theme import TOKYONIGHT_CSS
from ui.layout import FixedLayout
from ui.widgets.nav_tabs import NavTabs, NavItem
from ui.widgets.status_bar import StatusBar
from ui.widgets.control_bar import ControlBar

from ui.pages.hotseat import HotseatPage
from ui.pages.vs_ai import VsAIPage
from ui.pages.ai_vs_ai import AIVsAIPage
from ui.pages.replay import ReplayPage
from ui.pages.exit_page import ExitPage

from typing import TYPE_CHECKING, cast

if TYPE_CHECKING:
    from textual.widgets import Static


class PowerChessUI(App[None]):
    """Tokyonight-themed terminal UI for the 6x6 PowerChess RL project."""

    CSS = TOKYONIGHT_CSS
    TITLE = "PowerChess RL"
    BINDINGS = [
        ("q", "quit", "Quit"),
        ("ctrl+c", "quit", "Quit"),
        ("tab", "focus_next_pane", "Next Pane"),
        ("shift+tab", "focus_prev_pane", "Prev Pane"),
        ("left", "focus_nav", ""),  # quick jump to nav
        ("escape", "focus_nav", ""),  # escape also jumps to nav
        ("right", "focus_main", ""),  # quick jump back to main
    ]

    current_navigation_key: reactive[str] = reactive("hotseat")

    def compose(self) -> ComposeResult:
        control_bar: Widget = ControlBar()
        yield FixedLayout(
            items=[
                NavItem(key="hotseat", label="HOTSEAT", icon="▶"),
                NavItem(key="vsai", label="VS AI"),
                NavItem(key="aivai", label="AI v AI"),
                NavItem(key="replay", label="REPLAY"),
                NavItem(key="exit", label="EXIT"),
            ],
            active_key="hotseat",
            control_bar=control_bar,
            id="fixed-layout",
        )
        yield StatusBar(id="status")

    def on_ready(self) -> None:
        self._show_page("hotseat")
        self.action_focus_main()  # start in main

    @on(NavTabs.NavSelected)
    def handle_nav_selected(self, event: NavTabs.NavSelected) -> None:
        self._show_page(event.key)

    def _show_page(self, key: str) -> None:
        """Swap the central page (mount into #main) and update chrome."""
        self.current_navigation_key = key

        layout = self.query_one("#fixed-layout", FixedLayout)
        main_container = layout.query_one("#main")
        main_container.remove_children()

        page: Widget | None = None
        if key == "hotseat":
            page = HotseatPage()
        elif key == "vsai":
            page = VsAIPage()
        elif key == "aivai":
            page = AIVsAIPage()
        elif key == "replay":
            page = ReplayPage()
        elif key == "exit":
            page = ExitPage(on_confirm=lambda: self.exit())

        if page is not None:
            main_container.mount(page)

        # try focusing the board; if absent, focus the main container
        try:
            main_container.query_one("#board").focus()
        except Exception:
            main_container.focus()

        titlebar = cast("Static", self.query_one("#titlebar"))
        titlebar.update(f" PowerChess RL — {key.upper()} ")

        self.query_one(StatusBar).set_message(f"Page: {key}")

    # ── Pane focus management ─────────────────────────────────────────

    def _set_active_pane(self, main: bool = False, nav: bool = False, controls: bool = False) -> None:
        layout = self.query_one("#fixed-layout", FixedLayout)
        layout.query_one("#main-panel").set_class(main, "-active-pane")
        layout.query_one("#nav").set_class(nav, "-active-pane")
        try:
            self.query_one("ControlBar").set_class(controls, "-active-pane")
        except Exception:
            pass

    def action_focus_main(self) -> None:
        layout = self.query_one("#fixed-layout", FixedLayout)
        self._set_active_pane(main=True)
        # focus board if available
        try:
            layout.query_one("#main #board").focus()
        except Exception:
            layout.query_one("#main").focus()

    def action_focus_nav(self) -> None:
        layout = self.query_one("#fixed-layout", FixedLayout)
        self._set_active_pane(nav=True)
        # put focus on the active button so arrows/enter work immediately
        active = self.current_navigation_key
        try:
            layout.query_one(f"#nav #nav-{active}").focus()
        except Exception:
            layout.query_one("#nav").focus()

    def action_focus_controls(self) -> None:
        self._set_active_pane(controls=True)
        try:
            self.query_one("ControlBar").focus()
        except Exception:
            pass

    def _pane_widget(self, selector: str) -> Widget | None:
        try:
            return self.query_one(selector)
        except Exception:
            return None

    def _pane_contains_focus(self, selector: str) -> bool:
        root = self._pane_widget(selector)
        w = self.focused  # type: ignore[attr-defined]
        # Walk up the parents until None, checking identity
        while root is not None and w is not None:
            if w is root:
                return True
            # Some stubs don't expose .parent -> use getattr with fallback
            w = getattr(w, "parent", None)  # type: ignore[assignment]
        return False

    def action_focus_next_pane(self) -> None:
        # Cycle: main → nav → controls → main
        if self._pane_contains_focus("#main"):
            self.action_focus_nav()
        elif self._pane_contains_focus("#nav"):
            self.action_focus_controls()
        else:
            self.action_focus_main()

    def action_focus_prev_pane(self) -> None:
        # Reverse: controls → nav → main → controls
        if self._pane_contains_focus("ControlBar"):
            self.action_focus_nav()
        elif self._pane_contains_focus("#nav"):
            self.action_focus_main()
        else:
            self.action_focus_controls()

    async def action_quit(self) -> None:
        self.exit()


if __name__ == "__main__":
    PowerChessUI().run()
