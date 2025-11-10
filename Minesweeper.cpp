#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <ctime>
#include <string>

using namespace sf;

int main() {
    srand(time(0));
    const int w = 32;
    const int size = 10;
    const int mineCount = 20;
    const int totalRevealedToWin = (size * size) - mineCount;

    RenderWindow app(VideoMode(12 * w, 15 * w + 100), "Minesweeper");

    int grid[12][12];     // Hidden grid
    int sgrid[12][12];    // Shown grid
    int flagsPlaced = 0;
    int revealedCells = 0;

    bool gameOver = false;
    bool gameWon = false;

    // Load tile texture
    Texture t;
    if (!t.loadFromFile("images/tiles.jpg")) return -1;
    Sprite s(t);

    // Load font
    Font font;
    if (!font.loadFromFile("fonts/Inkfree.ttf")) return -1;

    // Game state text
    Text gameStateText;
    gameStateText.setFont(font);
    gameStateText.setCharacterSize(28);
    gameStateText.setFillColor(Color::Red);
    gameStateText.setString("");
    gameStateText.setPosition((app.getSize().x - 150) / 2.f, 16 * w + 10);

    // Text elements for score, flags, mines, timer
    Text scoreText, flagText, mineText, timerText;
    scoreText.setFont(font);
    flagText.setFont(font);
    mineText.setFont(font);
    timerText.setFont(font);
    scoreText.setCharacterSize(20);
    flagText.setCharacterSize(20);
    mineText.setCharacterSize(20);
    timerText.setCharacterSize(20);
    scoreText.setFillColor(Color::Black);
    flagText.setFillColor(Color::Black);
    mineText.setFillColor(Color::Black);
    timerText.setFillColor(Color::Black);

    // Load emoji faces
    Texture faceHappy, faceCool, faceDead;
    faceHappy.loadFromFile("images/smile.png");
    faceCool.loadFromFile("images/cool.png");
    faceDead.loadFromFile("images/dead.png");

    Sprite face(faceHappy);
    face.setScale(0.25f, 0.25f);
    FloatRect bounds = face.getGlobalBounds();
    face.setPosition((app.getSize().x - bounds.width) / 2.f, 0);

    // Restart button
    RectangleShape restartBtn(Vector2f(100, 30));
    restartBtn.setFillColor(Color(200, 200, 200));
    restartBtn.setPosition((app.getSize().x - 100) / 2.f, 12 * w + 60);

    Text restartText("Restart", font, 18);
    restartText.setFillColor(Color::Black);
    restartText.setPosition(restartBtn.getPosition().x + 15, restartBtn.getPosition().y + 5);

    // Hint message
    Text hintMessage;
    hintMessage.setFont(font);
    hintMessage.setCharacterSize(18);
    hintMessage.setFillColor(Color::Blue);
    hintMessage.setString("Press 'H' for a hint!");
    hintMessage.setPosition((app.getSize().x - 150) / 2.f, 12 * w + 100);  // Position it below the restart button

    // Clock for timer
    Clock clock;
    int elapsedTime = 0;

    auto resetGame = [&]() {
        revealedCells = 0;
        flagsPlaced = 0;
        gameOver = false;
        gameWon = false;
        gameStateText.setString("");  // Reset the game state text
        clock.restart();

        for (int i = 1; i <= size; i++)
            for (int j = 1; j <= size; j++) {
                grid[i][j] = 0;
                sgrid[i][j] = 10;
            }

        int placed = 0;
        while (placed < mineCount) {
            int x = rand() % size + 1;
            int y = rand() % size + 1;
            if (grid[x][y] != 9) {
                grid[x][y] = 9;
                placed++;
            }
        }

        for (int i = 1; i <= size; i++) {
            for (int j = 1; j <= size; j++) {
                if (grid[i][j] == 9) continue;
                int n = 0;
                for (int dx = -1; dx <= 1; dx++)
                    for (int dy = -1; dy <= 1; dy++)
                        if (grid[i + dx][j + dy] == 9)
                            n++;
                grid[i][j] = n;
            }
        }
        };

    resetGame();

    while (app.isOpen()) {
        elapsedTime = clock.getElapsedTime().asSeconds();

        Vector2i pos = Mouse::getPosition(app);
        int x = pos.x / w;
        int y = (pos.y - 40) / w;

        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if (e.type == Event::MouseButtonPressed) {
                Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                if (restartBtn.getGlobalBounds().contains(mousePos)) {
                    resetGame();
                }

                if (!gameOver && !gameWon && x >= 1 && x <= size && y >= 1 && y <= size) {
                    if (e.mouseButton.button == Mouse::Left) {
                        if (sgrid[x][y] == 10) {
                            sgrid[x][y] = grid[x][y];
                            if (grid[x][y] == 9) {
                                gameOver = true;
                                gameStateText.setString("Game Over!");  // Show Game Over message
                                for (int i = 1; i <= size; i++)
                                    for (int j = 1; j <= size; j++)
                                        sgrid[i][j] = grid[i][j];
                            }
                            else {
                                revealedCells++;
                            }
                        }
                    }
                    if (e.mouseButton.button == Mouse::Right) {
                        if (sgrid[x][y] == 10) {
                            sgrid[x][y] = 11;
                            flagsPlaced++;
                        }
                        else if (sgrid[x][y] == 11) {
                            sgrid[x][y] = 10;
                            flagsPlaced--;
                        }
                    }
                }
            }

            // Handle "H" key press for hint
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::H && !gameOver && !gameWon) {
                for (int i = 1; i <= size; i++) {
                    for (int j = 1; j <= size; j++) {
                        if (sgrid[i][j] == 10 && grid[i][j] != 9) {
                            sgrid[i][j] = grid[i][j];
                            revealedCells++;
                            break;
                        }
                    }
                }
            }
        }

        // Emoji update
        if (gameOver) {
            face.setTexture(faceDead);
        }
        else if (!gameWon && revealedCells == totalRevealedToWin) {
            gameWon = true;
            gameStateText.setString("You Win!");  // Show You Win message
            face.setTexture(faceCool);
        }
        else if (!gameOver && !gameWon) {
            face.setTexture(faceHappy);
        }

        // Scoreboard update
        scoreText.setString("Revealed: " + std::to_string(revealedCells));
        flagText.setString("Flags: " + std::to_string(flagsPlaced));
        mineText.setString("Mines: " + std::to_string(mineCount));
        timerText.setString("Time: " + std::to_string(elapsedTime) + "s");

        scoreText.setPosition(10, 12 * w + 20);
        flagText.setPosition(150, 12 * w + 20);
        mineText.setPosition(280, 12 * w + 20);
        timerText.setPosition(10, 12 * w + 60);

        // Drawing
        app.clear(Color::White);
        for (int i = 1; i <= size; i++) {
            for (int j = 1; j <= size; j++) {
                s.setTextureRect(IntRect(sgrid[i][j] * w, 0, w, w));
                s.setPosition(i * w, j * w + 40);
                app.draw(s);
            }
        }

        app.draw(scoreText);
        app.draw(flagText);
        app.draw(mineText);
        app.draw(timerText);
        app.draw(gameStateText);  // Draw the game state text (Game Over / You Win)
        app.draw(face);
        app.draw(restartBtn);
        app.draw(restartText);
        app.draw(hintMessage);  // Draw the hint message
        app.display();
    }

    return 0;
}