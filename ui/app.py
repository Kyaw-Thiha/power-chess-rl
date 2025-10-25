from __future__ import annotations

from textual.app import App, ComposeResult
from textual.containers import Horizontal, Vertical
from textual.reactive import reactive
from textual import on

from ui.theme import TOKYONIGHT_CSS
from ui.widgets.nav_tabs import NavTabs, NavItem
from ui.widgets.status_bar import StatusBar
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
    ]

    current_navigation_key: reactive[str] = reactive("hotseat")

    def compose(self) -> ComposeResult:
        yield Horizontal(
            Vertical(
                id="main",  # (kept; unused when using Screens)
            ),
            NavTabs(
                items=[
                    NavItem(key="hotseat", label="HOTSEAT", icon="▶"),
                    NavItem(key="vsai", label="VS AI"),
                    NavItem(key="aivai", label="AI v AI"),
                    NavItem(key="replay", label="REPLAY"),
                    NavItem(key="exit", label="EXIT"),
                ],
                id="nav",
                active_key="hotseat",
            ),
            id="root-row",
        )
        yield StatusBar(id="status")

    def on_mount(self) -> None:
        self.install_screen(HotseatPage(), name="hotseat")
        self.install_screen(VsAIPage(), name="vsai")
        self.install_screen(AIVsAIPage(), name="aivai")
        self.install_screen(ReplayPage(), name="replay")
        self.install_screen(ExitPage(on_confirm=lambda: self.exit()), name="exit")

        # FIRST show should be push_screen, not switch_screen (avoids pop-from-empty default stack)
        self.current_navigation_key = "hotseat"
        self.push_screen("hotseat")
        self.query_one(StatusBar).set_message("Page: hotseat")

    @on(NavTabs.NavSelected)
    def handle_nav_selected(self, event: NavTabs.NavSelected) -> None:
        self._show_page(event.key)

    def _show_page(self, key: str) -> None:
        """Swap the central page based on `key`."""
        self.current_navigation_key = key
        # For subsequent navigations, switch between installed screens
        self.switch_screen(key)
        self.query_one(StatusBar).set_message(f"Page: {key}")

    async def action_quit(self) -> None:
        self.exit()


if __name__ == "__main__":
    PowerChessUI().run()
