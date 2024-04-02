#pragma once

enum class AppStateEnum {
	EDITOR,
	GAME,
	PAUSED
};

class AppState {
public:
	void Play() {
		if (isEditor()) {
			m_CurrentState = AppStateEnum::GAME;
			m_SwitchedToGame = true;
		}
	}
	void Pause() {
		if (isGame()) {
			m_CurrentState = AppStateEnum::PAUSED;
		}
		else if (isPaused()) {
			m_CurrentState = AppStateEnum::GAME;
		}
	}
	void Stop() {
		if (isRuntime()) {
			m_CurrentState = AppStateEnum::EDITOR;
			m_SwitchedToEditor = true;
		}
	}

	bool isEditor() {
		return m_CurrentState == AppStateEnum::EDITOR;
	}
	bool isGame() {
		return m_CurrentState == AppStateEnum::GAME;
	}
	bool isPaused() {
		return m_CurrentState == AppStateEnum::PAUSED;
	}
	bool isRuntime() {
		return isGame() || isPaused();
	}

	bool SwitchedToEditorConsume() {
		bool state = false;
		std::swap(state, m_SwitchedToEditor);
		return state;
	}
	bool SwitchedToGameConsume() {
		bool state = false;
		std::swap(state, m_SwitchedToGame);
		return state;
	}

	AppState() = default;
	AppState(AppStateEnum startState) : m_CurrentState{ startState } {};

private:
	AppStateEnum m_CurrentState = AppStateEnum::EDITOR;
	bool m_SwitchedToEditor = false;
	bool m_SwitchedToGame = false;
};