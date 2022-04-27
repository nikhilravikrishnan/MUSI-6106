#include "BoardComponent.h"

BoardComponent::BoardComponent() {
    std::pair<Chess::Piece::Type, std::string> types[] = {
        {Chess::Piece::Type::Pawn, "Pawn"},
        {Chess::Piece::Type::Knight, "Knight"},
        {Chess::Piece::Type::Bishop, "Bishop"},
        {Chess::Piece::Type::Rook, "Rook"},
        {Chess::Piece::Type::Queen, "Queen"},
        {Chess::Piece::Type::King, "King"},
    };
    std::pair<Chess::Color, std::string> colors[] = {{Chess::Color::White, "W"}, {Chess::Color::Black, "B"}};

    for (auto &[type, typeName] : types) {
        for (auto &[color, colorName] : colors) {
            int size;
            std::string resourceName = colorName + "_" + typeName + "_png";
            const char *data = ChessImageData::getNamedResource(resourceName.c_str(), size);
            pieceImages.emplace(Chess::Piece(type, color), juce::ImageFileFormat::loadFrom(data, size));
        }
    }
}

void BoardComponent::paint(juce::Graphics &g) {
    static const juce::Colour lightSquare(240, 217, 181);
    static const juce::Colour darkSquare(181, 136, 99);

    // Draw squares.
    // NOTE: We assume our parent is maintaining our 1:1 aspect ratio.
    int size = getWidth();
    float squareSize = getSquareSize();
    g.fillCheckerBoard(juce::Rectangle<float>(0, 0, size, size), squareSize, squareSize, lightSquare, darkSquare);

    if (selected) {
        g.setColour(juce::Colours::red.withAlpha(0.5f));
        g.fillRect(squareToRect(*selected));
        // TODO: Maybe cache this.
        auto candidates = AppState::getInstance().getGame().generateMoves(*selected);
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        for (auto move : candidates) {
            g.fillRect(squareToRect(move.dst));
        }
        g.setColour(juce::Colours::black);
    }

    // Draw pieces.
    float pieceHeight = squareSize * .8;
    float topMargin = (squareSize - pieceHeight) / 2.0;
    for (auto [square, piece] : AppState::getInstance().getGame().getPieceMap()) {
        float x = squareSize * square.file;
        float y = squareSize * (7 - square.rank);
        juce::Image &image = pieceImages[piece];
        float pieceWidth = pieceHeight * image.getWidth() / image.getHeight();
        float leftMargin = (squareSize - pieceWidth) / 2.0;
        g.drawImage(image, juce::Rectangle<float>(x + leftMargin, y + topMargin, pieceWidth, pieceHeight));
    }
}

float BoardComponent::getSquareSize() const {
    return (float)getWidth() / BoardSize;
}

Chess::Square BoardComponent::coordsToSquare(int x, int y) const {
    float squareSize = getSquareSize();
    return Chess::Square(7 - (int)(y / squareSize), (int)(x / squareSize));
}

juce::Rectangle<float> BoardComponent::squareToRect(Chess::Square square) const {
    float squareSize = getSquareSize();
    float x = squareSize * square.file;
    float y = squareSize * (7 - square.rank);
    return juce::Rectangle<float>(x, y, squareSize, squareSize);
}

void BoardComponent::mouseDrag(const juce::MouseEvent &event) {
    std::cout << "drag! " << event.x << " " << event.y << std::endl;
}

void BoardComponent::mouseDown(const juce::MouseEvent &event) {
    if (m_CurrentMode == Mode::PGN) return;

    Chess::Square clicked = coordsToSquare(event.x, event.y);
    Chess::Game &game = AppState::getInstance().getGame();
    Chess::Color turn = game.getTurn();
    std::optional<Chess::Piece> piece = game.getPieceAt(clicked);

    std::cout << "down! " << event.x << " " << event.y << " " << clicked.toString() << std::endl;

    if (selected) {
        if (clicked == *selected) {
            onStateChange(State::Idle);
            selected.reset();
        } else if (piece && game.getPieceAt(*selected)->color == piece->color) {
            onStateChange(State::Switching);
            // selectPiece(piece);
            selected = clicked;
        } else {
            juce::String intent = selected->toString() + clicked.toString(); // m_SelectedPiece->getSquareId() + piece.getSquareId();
            sendActionMessage(intent);
            selected.reset();
        }
    } else {
        if (m_CurrentMode == Mode::PVP) {
            if (piece && turn == piece->color && (m_CurrentMode == Mode::PVP || turn == Chess::Color::White)) {
                // selectPiece(piece);
                onStateChange(State::Placing);
                selected = clicked;
            }
        }
    }

    // sendActionMessage("Preview " + getId()) ?
}

void BoardComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    onStateChange(State::Idle);
}

void BoardComponent::actionListenerCallback(const juce::String &message) {
    if (message.contains("Preview")) {
        // if (m_SelectedPiece) {
        //     juce::String newMessage = message.substring(0, 8) +
        //                               m_SelectedPiece->getSquareId() +
        //                               message.substring(8, 10);
        //     sendActionMessage(newMessage);
        // }
    }
}

void BoardComponent::onModeChange(BoardComponent::Mode newMode) {
    switch (newMode) {
    case Mode::PVP:
        break;
    case Mode::PVC:
        break;
    default:;
    }
    m_CurrentMode = newMode;
}

// void BoardComponent::selectPiece(Piece &piece) {
//     if (m_SelectedPiece) {
//         if (m_SelectedPiece->getId() != piece.getId())
//             sendActionMessage("Deselect " + m_SelectedPiece->getId());
//     }
//     sendActionMessage("Select " + m_SelectedPiece->getId());
//     onStateChange(State::Placing);
// }

// BoardComponent::Square *
// BoardComponent::findSquare(const std::string &squareId) const {
//     for (int row = 0; row < BoardSize; row++) {
//         for (int col = 0; col < BoardSize; col++) {
//             Square *square = m_AllSquares[row][col];
//             if (squareId == square->getId())
//                 return square;
//         }
//     }
// }

// BoardComponent::Square *BoardComponent::findSquare(const Piece &piece) const {
//     return findSquare(piece.getSquareId());
// }

// void BoardComponent::highlightPossibleMoves(const Piece *piece) {
//     if (piece) {
//         Chess::Game game = AppState::getInstance().getGame();
//         std::unordered_set moves =
//             game.generateMoves(Chess::Square(piece->getSquareId()));
//         for (const Chess::Move &move : moves) {
//             Square *square = findSquare(move.dst.toString());
//             square->isCandidate(true);
//         }
//     }
// }

void BoardComponent::resetPossibleMoves() {
    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            // Square *&square = m_AllSquares[row][col];
            // square->isCandidate(false);
        }
    }
}

void BoardComponent::onStateChange(BoardComponent::State newState) {
    switch (newState) {
    case State::Idle:
        // if (m_SelectedPiece) {
        //     m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
        //     m_SelectedPiece->setSelected(false);
        //     sendActionMessage("Deselect " + m_SelectedPiece->getId());
        // }
        // m_SelectedPiece = nullptr;
        resetPossibleMoves();
        break;
    case State::Switching:
        resetPossibleMoves();
        break;
    default:;
    }

    m_CurrentState = newState;
    repaint();
}
