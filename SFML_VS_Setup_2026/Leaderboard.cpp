#include "Leaderboard.h"
#include <cstdlib>   // rand, srand
#include <cmath>     // sinf

// ================================================================
//  CONSTRUCTOR
// ================================================================
Leaderboard::Leaderboard()
    : mCount(0),
    mFilePath("data/leaderboard.txt"),
    mFontLoaded(false),
    mScrollOffset(0),
    mAnimTimer(0.f),
    mBgScrollX(0.f),
    mScrollCooldown(0.f)
{
    initFlakes();
    load();
}

// ================================================================
//  SNOWFLAKE INIT
// ================================================================
void Leaderboard::initFlakes() {
    srand((unsigned)time(nullptr));
    for (int i = 0; i < FLAKE_COUNT; i++) {
        mFlakes[i].x = (float)(rand() % 800);
        mFlakes[i].y = (float)(rand() % 660);
        mFlakes[i].speed = 20.f + (rand() % 60);
        mFlakes[i].size = 1.f + (rand() % 3);
    }
}

// ================================================================
//  FONT
// ================================================================
void Leaderboard::setFont(const sf::Font& font) {
    mFont = font;
    mFontLoaded = true;
}

// ================================================================
//  TIMESTAMP  "YYYY-MM-DD HH:MM"
// ================================================================
string Leaderboard::currentTimestamp() const {
   time_t now = time(nullptr);
tm t_buf;
localtime_s(&t_buf, &now);
tm* t = &t_buf;
    char   buf[32];
    // manual formatting — no strftime to avoid locale issues
    int yr = t->tm_year + 1900;
    int mo = t->tm_mon + 1;
    int dy = t->tm_mday;
    int hr = t->tm_hour;
    int mn = t->tm_min;

    // zero-pad manually
    buf[0] = '0' + (yr / 1000);
    buf[1] = '0' + (yr / 100 % 10);
    buf[2] = '0' + (yr / 10 % 10);
    buf[3] = '0' + (yr % 10);
    buf[4] = '-';
    buf[5] = '0' + (mo / 10);
    buf[6] = '0' + (mo % 10);
    buf[7] = '-';
    buf[8] = '0' + (dy / 10);
    buf[9] = '0' + (dy % 10);
    buf[10] = ' ';
    buf[11] = '0' + (hr / 10);
    buf[12] = '0' + (hr % 10);
    buf[13] = ':';
    buf[14] = '0' + (mn / 10);
    buf[15] = '0' + (mn % 10);
    buf[16] = '\0';
    return string(buf);
}

// ================================================================
//  SORT  (insertion-sort, descending score)
// ================================================================
void Leaderboard::sortEntries() {
    for (int i = 1; i < mCount; i++) {
        LeaderboardEntry key = mEntries[i];
        int j = i - 1;
        while (j >= 0 && mEntries[j].score < key.score) {
            mEntries[j + 1] = mEntries[j];
            j--;
        }
        mEntries[j + 1] = key;
    }
}

void Leaderboard::trimToMax() {
    if (mCount > MAX_ENTRIES)
        mCount = MAX_ENTRIES;
}

// ================================================================
//  ADD ENTRY
// ================================================================
void Leaderboard::addEntry(const string& username, int score, int level) {
    if (mCount < MAX_ENTRIES) {
        mEntries[mCount].username = username;
        mEntries[mCount].score = score;
        mEntries[mCount].level = level;
        mEntries[mCount].timestamp = currentTimestamp();
        mCount++;
    }
    else {
        // Only replace the lowest entry if this score is higher
        int lowestIdx = 0;
        for (int i = 1; i < mCount; i++)
            if (mEntries[i].score < mEntries[lowestIdx].score)
                lowestIdx = i;

        if (score > mEntries[lowestIdx].score) {
            mEntries[lowestIdx].username = username;
            mEntries[lowestIdx].score = score;
            mEntries[lowestIdx].level = level;
            mEntries[lowestIdx].timestamp = currentTimestamp();
        }
    }
    sortEntries();
    trimToMax();
    save();
}

// ================================================================
//  LOAD
// ================================================================
void Leaderboard::load() {
    mCount = 0;
    ifstream file(mFilePath.c_str());
    if (!file.is_open()) return;

    string line;
    while (getline(file, line) && mCount < MAX_ENTRIES) {
        if (line.empty()) continue;

        // parse:  username|score|level|timestamp
        LeaderboardEntry e;
        istringstream ss(line);
        string token;

        if (!getline(ss, token, '|')) continue;
        e.username = token;

        if (!getline(ss, token, '|')) continue;
        e.score = atoi(token.c_str());

        if (!getline(ss, token, '|')) continue;
        e.level = atoi(token.c_str());

        if (!getline(ss, token, '|')) continue;
        e.timestamp = token;

        mEntries[mCount++] = e;
    }
    file.close();
    sortEntries();
}

// ================================================================
//  SAVE
// ================================================================
void Leaderboard::save() const {
    ofstream file(mFilePath.c_str());
    if (!file.is_open()) return;

    for (int i = 0; i < mCount; i++) {
        file << mEntries[i].username << '|'
            << mEntries[i].score << '|'
            << mEntries[i].level << '|'
            << mEntries[i].timestamp << '\n';
    }
    file.close();
}

// ================================================================
//  RANK OF
// ================================================================
int Leaderboard::getRankOf(const string& username) const {
    for (int i = 0; i < mCount; i++)
        if (mEntries[i].username == username)
            return i + 1;   // 1-based
    return -1;
}

// ================================================================
//  BACKGROUND + SNOWFLAKES
// ================================================================
void Leaderboard::updateFlakes(float dt) {
    for (int i = 0; i < FLAKE_COUNT; i++) {
        mFlakes[i].y += mFlakes[i].speed * dt;
        mFlakes[i].x += sinf(mFlakes[i].y * 0.03f) * 0.4f;
        if (mFlakes[i].y > 680.f) {
            mFlakes[i].y = -5.f;
            mFlakes[i].x = (float)(rand() % 800);
        }
    }
}

void Leaderboard::drawBackground(sf::RenderWindow& w, float dt) {
   

    // Snowflakes
    updateFlakes(dt);
    for (int i = 0; i < FLAKE_COUNT; i++) {
        sf::CircleShape flake(mFlakes[i].size);
        flake.setPosition(mFlakes[i].x, mFlakes[i].y);
        flake.setFillColor(sf::Color(200, 220, 255, 160));
        w.draw(flake);
    }

    // Horizontal divider beneath title area
    sf::RectangleShape divTop(sf::Vector2f(760.f, 2.f));
    divTop.setPosition(20.f, 105.f);
    divTop.setFillColor(sf::Color(80, 140, 255, 180));
    w.draw(divTop);

    // Bottom divider
    sf::RectangleShape divBot(sf::Vector2f(760.f, 2.f));
    divBot.setPosition(20.f, 620.f);
    divBot.setFillColor(sf::Color(80, 140, 255, 180));
    w.draw(divBot);
}

// ================================================================
//  TITLE
// ================================================================
void Leaderboard::drawTitle(sf::RenderWindow& w) {
    if (!mFontLoaded) return;

    // Shadow
    sf::Text shadow;
    shadow.setFont(mFont);
    shadow.setString("LEADERBOARD");
    shadow.setCharacterSize(48);
    shadow.setFillColor(sf::Color(0, 80, 160, 120));
    float sw = shadow.getGlobalBounds().width;
    shadow.setPosition((800.f - sw) / 2.f + 3.f, 23.f);
    w.draw(shadow);

    // Main title — ice-blue gradient trick via two overlapping texts
    sf::Text title;
    title.setFont(mFont);
    title.setString("LEADERBOARD");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color(100, 200, 255));
    float tw = title.getGlobalBounds().width;
    title.setPosition((800.f - tw) / 2.f, 20.f);
    w.draw(title);

    // Trophy icon substitute — star chars
    sf::Text star;
    star.setFont(mFont);
    star.setString("* TOP SCORES *");
    star.setCharacterSize(22);
    star.setFillColor(sf::Color(255, 215, 0, 200));
    float starW = star.getGlobalBounds().width;
    star.setPosition((800.f - starW) / 2.f, 72.f);
    w.draw(star);
}

// ================================================================
//  TABLE
// ================================================================
void Leaderboard::drawTable(sf::RenderWindow& w) {
    if (!mFontLoaded) return;

    // Column header bar
    sf::RectangleShape headerBar(sf::Vector2f(760.f, 28.f));
    headerBar.setPosition(20.f, 110.f);
    headerBar.setFillColor(sf::Color(20, 50, 120, 220));
    w.draw(headerBar);

    // Column headers
    const float colRank = 30.f;
    const float colName = 90.f;
    const float colScore = 330.f;
    const float colLevel = 510.f;
    const float colTime = 600.f;
    const float headerY = 113.f;

    auto makeHeader = [&](const string& txt, float x) {
        sf::Text t;
        t.setFont(mFont);
        t.setString(txt);
        t.setCharacterSize(17);
        t.setFillColor(sf::Color(180, 220, 255));
        t.setPosition(x, headerY);
        w.draw(t);
        };
    makeHeader("RANK", colRank);
    makeHeader("PLAYER", colName);
    makeHeader("                    SCORE", colScore);
    makeHeader("LEVEL", colLevel);
    makeHeader("DATE", colTime);

    // Row separator line
    sf::RectangleShape sep(sf::Vector2f(760.f, 1.f));
    sep.setFillColor(sf::Color(60, 100, 200, 120));

    int visible = DISPLAY_ROWS;
    int end = mScrollOffset + visible;
    if (end > mCount) end = mCount;

    for (int i = mScrollOffset; i < end; i++) {
        int   row = i - mScrollOffset;
        float rowY = 145.f + row * 46.f;
        int   rank = i + 1;

        // Row background  — alternating shades, gold for #1
        sf::RectangleShape rowBg(sf::Vector2f(760.f, 42.f));
        rowBg.setPosition(20.f, rowY - 2.f);

        if (rank == 1) {
            // Animated gold pulse for 1st place
            float pulse = 0.5f + 0.5f * sinf(mAnimTimer * 2.5f);
            sf::Uint8 alpha = (sf::Uint8)(80 + (int)(40 * pulse));
            rowBg.setFillColor(sf::Color(120, 90, 0, alpha));
        }
        else if (row % 2 == 0) {
            rowBg.setFillColor(sf::Color(15, 30, 70, 180));
        }
        else {
            rowBg.setFillColor(sf::Color(10, 20, 50, 180));
        }
        w.draw(rowBg);

        // Row border highlight for top 3
        if (rank <= 3) {
            sf::RectangleShape leftBar(sf::Vector2f(4.f, 42.f));
            leftBar.setPosition(20.f, rowY - 2.f);
            sf::Color barCol = (rank == 1) ? sf::Color(255, 215, 0)
                : (rank == 2) ? sf::Color(192, 192, 192)
                : sf::Color(205, 127, 50);
            leftBar.setFillColor(barCol);
            w.draw(leftBar);
        }

        // Rank number
        sf::Color rankCol = sf::Color::White;
        if (rank == 1) rankCol = sf::Color(255, 215, 0);
        else if (rank == 2) rankCol = sf::Color(210, 210, 210);
        else if (rank == 3) rankCol = sf::Color(205, 127, 50);

        sf::Text rankT;
        rankT.setFont(mFont);
        rankT.setString("#" + to_string(rank));
        rankT.setCharacterSize(20);
        rankT.setFillColor(rankCol);
        rankT.setPosition(colRank, rowY + 8.f);
        w.draw(rankT);

        // Medal for top 3
        if (rank <= 3) {
            string medal = "";
            sf::Text medT;
            medT.setFont(mFont);
            medT.setString(medal);
            medT.setCharacterSize(16);
            medT.setFillColor(rankCol);
            medT.setPosition(colRank, rowY + 26.f);
            w.draw(medT);
        }

        // Username
        string uname = mEntries[i].username;
        if ((int)uname.size() > 14) uname = uname.substr(0, 13) + ".";
        sf::Text nameT;
        nameT.setFont(mFont);
        nameT.setString(uname);
        nameT.setCharacterSize(19);
        nameT.setFillColor(sf::Color(220, 240, 255));
        nameT.setPosition(colName, rowY + 10.f);
        w.draw(nameT);

        // Score  — right-aligned in its column
        string scoreStr = to_string(mEntries[i].score);
        sf::Text scoreT;
        scoreT.setFont(mFont);
        scoreT.setString(scoreStr);
        scoreT.setCharacterSize(19);
        scoreT.setFillColor(sf::Color(100, 255, 180));
        // right-align: place so right edge is at colScore + 120
        float scoreRight = colScore + 120.f;
        scoreT.setPosition(scoreRight - scoreT.getGlobalBounds().width, rowY + 10.f);
        w.draw(scoreT);

        // Level
        sf::Text levelT;
        levelT.setFont(mFont);
        levelT.setString("LV " + to_string(mEntries[i].level));
        levelT.setCharacterSize(19);
        levelT.setFillColor(sf::Color(180, 140, 255));
        levelT.setPosition(colLevel, rowY + 10.f);
        w.draw(levelT);

        // Timestamp — smaller font
        string ts = mEntries[i].timestamp;
        if ((int)ts.size() > 16) ts = ts.substr(0, 16);
        sf::Text timeT;
        timeT.setFont(mFont);
        timeT.setString(ts);
        timeT.setCharacterSize(15);
        timeT.setFillColor(sf::Color(140, 160, 180));
        timeT.setPosition(colTime, rowY + 12.f);
        w.draw(timeT);

        // Separator line
        sep.setPosition(20.f, rowY + 42.f);
        w.draw(sep);
    }

    // Empty-state message
    if (mCount == 0) {
        sf::Text empty;
        empty.setFont(mFont);
        empty.setString("NO SCORES YET — PLAY TO BE FIRST!");
        empty.setCharacterSize(22);
        empty.setFillColor(sf::Color(150, 170, 200));
        float ew = empty.getGlobalBounds().width;
        empty.setPosition((800.f - ew) / 2.f, 300.f);
        w.draw(empty);
    }
}

// ================================================================
//  SCROLL HINTS
// ================================================================
void Leaderboard::drawScrollHints(sf::RenderWindow& w) {
    if (!mFontLoaded) return;

    if (mScrollOffset > 0) {
        sf::Text up;
        up.setFont(mFont);
        up.setString("");
        up.setCharacterSize(17);
        up.setFillColor(sf::Color::White);
        up.setPosition(370.f, 113.f);
        w.draw(up);
    }

    if (mScrollOffset + DISPLAY_ROWS < mCount) {
        sf::Text dn;
        dn.setFont(mFont);
        dn.setString("");
        dn.setCharacterSize(17);
        dn.setFillColor(sf::Color::White);
        dn.setPosition(362.f, 607.f);
        w.draw(dn);
    }
}

// ================================================================
//  FOOTER
// ================================================================
void Leaderboard::drawFooter(sf::RenderWindow& w) {
    if (!mFontLoaded) return;

    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("UP/DOWN TO SCROLL    ESC TO RETURN");
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color(255,255,255));
    float hw = hint.getGlobalBounds().width;
    hint.setPosition((800.f - hw) / 2.f, 628.f);
    w.draw(hint);
}

// ================================================================
//  MAIN DRAW  (returns true when user wants to leave)
// ================================================================
bool Leaderboard::draw(sf::RenderWindow& window, float deltaTime) {
    mAnimTimer += deltaTime;
    mScrollCooldown = mScrollCooldown > 0.f
        ? mScrollCooldown - deltaTime : 0.f;

    // Handle scroll input
    if (mScrollCooldown <= 0.f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (mScrollOffset + DISPLAY_ROWS < mCount) {
                mScrollOffset++;
                mScrollCooldown = 0.18f;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            if (mScrollOffset > 0) {
                mScrollOffset--;
                mScrollCooldown = 0.18f;
            }
        }
    }

    // Draw layers
    drawBackground(window, deltaTime);
    drawTitle(window);
    drawTable(window);
    drawScrollHints(window);
    drawFooter(window);

    // Back navigation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
        return true;

    return false;
}