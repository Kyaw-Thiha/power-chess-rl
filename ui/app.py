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

# Page widgets (they should be Widgets, not Screens)
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
        # If you have a real ControlBar widget, pass it here; otherwise pass None.
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

        # Keep StatusBar last so it docks at the very bottom under the control bar.
        yield StatusBar(id="status")

    def on_mount(self) -> None:
        # No initial render here; layout isn't ready yet.
        pass

    def on_ready(self) -> None:
        self._show_page("hotseat")

    @on(NavTabs.NavSelected)
    def handle_nav_selected(self, event: NavTabs.NavSelected) -> None:
        self._show_page(event.key)

    def _show_page(self, key: str) -> None:
        """Swap the central page (mount into #main)."""
        self.current_navigation_key = key

        main_container = self.query_one("#main")
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

        self.query_one(StatusBar).set_message(f"Page: {key}")

    async def action_quit(self) -> None:
        self.exit()


if __name__ == "__main__":
    PowerChessUI().run()
