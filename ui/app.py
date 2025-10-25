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


class PowerChessUI(App[None]):
    """Tokyonight-themed terminal UI for the 6x6 PowerChess RL project."""

    CSS = TOKYONIGHT_CSS
    TITLE = "PowerChess RL"
    BINDINGS = [
        ("q", "quit", "Quit"),
        ("ctrl+c", "quit", "Quit"),
        ("tab", "focus_next_pane", "Next Pane"),
        ("shift+tab", "focus_prev_pane", "Prev Pane"),
        ("left", "focus_nav", ""),  # quick shortcut to nav
        ("escape", "focus_nav", ""),  # escape also jumps to nav
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
        self.action_focus_main()  # start with main focused

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

        # Orange border on active tab
        main_panel = layout.query_one("#main-panel")
        main_panel.set_class(True, "-active-tab")

        # Focus the board if present, otherwise keep focus in main
        try:
            main_container.query_one("#board").focus()
        except Exception:
            main_container.focus()

        self.query_one(StatusBar).set_message(f"Page: {key}")

    # ── Pane focus actions (Tab cycling & shortcuts) ──────────────────
    def action_focus_nav(self) -> None:
        try:
            self.query_one("#nav").focus()
            # also focus the active button so arrow keys work immediately
            active = self.current_navigation_key
            self.query_one(f"#nav #nav-{active}").focus()
        except Exception:
            pass

    def action_focus_main(self) -> None:
        try:
            self.query_one("#main").focus()
            # if there's a board, focus it
            self.query_one("#main #board").focus()
        except Exception:
            pass

    def action_focus_controls(self) -> None:
        try:
            self.query_one("ControlBar").focus()
        except Exception:
            pass

    def action_focus_next_pane(self) -> None:
        # Cycle: main -> nav -> controls -> main
        anchors = [self.action_focus_main, self.action_focus_nav, self.action_focus_controls]
        self._cycle_focus(anchors, forward=True)

    def action_focus_prev_pane(self) -> None:
        anchors = [self.action_focus_main, self.action_focus_controls, self.action_focus_nav]
        self._cycle_focus(anchors, forward=True)

    def _cycle_focus(self, anchors, forward: bool) -> None:
        # Try each anchor in order until one focuses something.
        for fn in anchors:
            before = self.screen.focused is not None
            fn()
            after = self.screen.focused is not None
            if after and not before:
                return

    async def action_quit(self) -> None:
        self.exit()


if __name__ == "__main__":
    PowerChessUI().run()
