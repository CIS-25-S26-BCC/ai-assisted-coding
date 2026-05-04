#include "src/map.h"
#include "src/player.h"
#include "src/renderer.h"
#include "src/types.h"

#include <chrono>
#include <cmath>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <thread>
#include <unistd.h>

namespace {

termios g_originalTermios {};
int g_originalFlags = 0;
bool g_rawModeEnabled = false;
volatile std::sig_atomic_t g_interrupted = 0;

void disableRawMode() {
    if (!g_rawModeEnabled) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_originalTermios);
    fcntl(STDIN_FILENO, F_SETFL, g_originalFlags);
    std::cout << "\033[?25h" << std::flush;
    g_rawModeEnabled = false;
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &g_originalTermios) == -1) {
        return;
    }

    g_originalFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    termios raw = g_originalTermios;
    raw.c_lflag &= static_cast<unsigned long>(~(ECHO | ICANON));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return;
    }

    fcntl(STDIN_FILENO, F_SETFL, g_originalFlags | O_NONBLOCK);
    std::atexit(disableRawMode);
    std::cout << "\033[2J\033[H\033[?25l" << std::flush;
    g_rawModeEnabled = true;
}

void onSignal(int /*signum*/) {
    g_interrupted = 1;
}

char readKeyNonBlocking() {
    char ch = '\0';
    const ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) {
        return ch;
    }
    return '\0';
}

bool playerOnGoal(const Player& player, const Map& map) {
    const int col = static_cast<int>(std::floor(player.x));
    const int row = static_cast<int>(std::floor(player.y));
    return map.tileAt(col, row) == TileType::Goal;
}

} // namespace

int main() {
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    enableRawMode();

    const std::string level =
        "############################\n"
        "#.........................G#\n"
        "#..................###.....#\n"
        "#..........X...............#\n"
        "#.....###..................#\n"
        "#S........................##\n"
        "############################\n";

    Map map(level);
    int spawnCol = 0, spawnRow = 0;
    map.spawnPosition(spawnCol, spawnRow);

    Player player(static_cast<float>(spawnCol), static_cast<float>(spawnRow));
    Renderer renderer;
    GameState state = GameState::Running;

    using clock = std::chrono::steady_clock;
    constexpr float fixedDt = 1.0f / 30.0f;

    auto previous = clock::now();
    float accumulator = 0.0f;

    while (state != GameState::Quit && state != GameState::Won) {
        if (g_interrupted) {
            state = GameState::Quit;
            break;
        }

        const auto now = clock::now();
        const std::chrono::duration<float> frameDelta = now - previous;
        previous = now;
        accumulator += frameDelta.count();

        const char key = readKeyNonBlocking();
        if (key == 'q' || key == 'Q') {
            state = GameState::Quit;
            break;
        }

        player.applyInput(key);

        while (accumulator >= fixedDt && state == GameState::Running) {
            player.update(fixedDt, map);
            accumulator -= fixedDt;

            if (player.isDead()) {
                state = GameState::Dead;
                break;
            }
            if (playerOnGoal(player, map)) {
                state = GameState::Won;
                break;
            }
        }

        renderer.draw(map, player, state);

        if (state == GameState::Dead) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            player.respawn();
            state = GameState::Running;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    if (state == GameState::Won) {
        renderer.draw(map, player, state);
        std::cout << "You won! Press Q to exit.\n" << std::flush;
        while (true) {
            if (g_interrupted) {
                break;
            }
            const char key = readKeyNonBlocking();
            if (key == 'q' || key == 'Q') {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    disableRawMode();
    return 0;
}
