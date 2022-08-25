// Functions to manage both the layout and user inputs of the video overlay
// It's very artisanal and would usually be done with a framework like
// React, but it's enough for the needs of the project.

const PROGRESS_BAR_COLOR = "#2a81d4";
const PROGRESS_BAR_BG_COLOR = "#2d2d2d";
const VOLUME_BAR_COLOR = "#4ad24a";
const VOLUME_BAR_MUTE_COLOR = "#4d704e";
const VOLUME_BAR_BG_COLOR = "#2d2d2d";

const BUTTON_BG_COLOR = "#464646";
const GENERAL_BG_COLOR = "#353535";
const TEXT_COLOR = "#8d8d8c";

const MENU_BAR_HEIGHT = 30;
const BUTTON_SIZE = 20;
const GAP_SIZE = 5;

function get_layout(canvas) {
  const VOLUME_BAR_WIDTH = 100;
  const VOLUME_BAR_HEIGHT = 20;

  const PROGRESS_TEXT_WIDTH = 80;

  const PROGRESS_BAR_WIDTH = canvas.width -
  (GAP_SIZE + BUTTON_SIZE + GAP_SIZE + GAP_SIZE + BUTTON_SIZE + GAP_SIZE + PROGRESS_TEXT_WIDTH + GAP_SIZE + VOLUME_BAR_WIDTH + GAP_SIZE);
  const PROGRESS_BAR_HEIGHT = 10;

  const PLAY_BUTTON_X = GAP_SIZE;
  const PROGRESS_BAR_X = PLAY_BUTTON_X + BUTTON_SIZE + GAP_SIZE;
  const PROGRESS_TEXT_X = PROGRESS_BAR_X + PROGRESS_BAR_WIDTH + GAP_SIZE;
  const VOLUME_BUTTON_X = PROGRESS_TEXT_X + PROGRESS_TEXT_WIDTH + GAP_SIZE;
  const VOLUME_BAR_X = VOLUME_BUTTON_X + BUTTON_SIZE + GAP_SIZE;

  return {
    VOLUME_BAR_WIDTH,
    VOLUME_BAR_HEIGHT,
    PROGRESS_TEXT_WIDTH,
    PROGRESS_BAR_WIDTH,
    PROGRESS_BAR_HEIGHT,
    PLAY_BUTTON_X,
    PROGRESS_BAR_X,
    PROGRESS_TEXT_X,
    VOLUME_BUTTON_X,
    VOLUME_BAR_X,
  };
}

const central_play_icon = new Image(50, 50);
central_play_icon.src = "vlc/modules/gui/qt/pixmaps/play_button.svg";

const play_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
play_button.src = "vlc/modules/gui/qt/pixmaps/play.png";
const pause_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
pause_button.src = "vlc/modules/gui/qt/pixmaps/pause.png";

const volume_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
volume_button.src = "vlc/modules/gui/qt/pixmaps/toolbar/volume-medium.png";
const muted_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
muted_button.src = "vlc/modules/gui/qt/pixmaps/toolbar/volume-muted.png";


const filepicker_button = document.createElement("input");
filepicker_button.id = "fpicker_btn";
filepicker_button.type = "file";
filepicker_button.style = "display:none";
document.body.appendChild(filepicker_button);
// Apply underlying changes (eg video is paused) to the displayed UI
export function update_overlay(overlay) {
  const ctx = overlay.getContext("2d");
  const media_player = window.media_player;
  if (media_player == null) {
    return;
  }

  const {
    VOLUME_BAR_WIDTH,
    VOLUME_BAR_HEIGHT,
    PROGRESS_TEXT_WIDTH,
    PROGRESS_BAR_WIDTH,
    PROGRESS_BAR_HEIGHT,
    PLAY_BUTTON_X,
    PROGRESS_BAR_X,
    PROGRESS_TEXT_X,
    VOLUME_BUTTON_X,
    VOLUME_BAR_X,
  } = get_layout(overlay);

  ctx.save();
  ctx.clearRect(0, 0, overlay.width, overlay.height);

  let is_paused = !media_player.is_playing();

  if (is_paused) {
    ctx.drawImage(
      central_play_icon,
      overlay.width / 2 - central_play_icon.width / 2,
      overlay.height / 2 - central_play_icon.height / 2,
      central_play_icon.width,
      central_play_icon.height,
    );
  }

  if (window.display_overlay) {
    // -- PAINT BACKGROUND --
    ctx.fillStyle = GENERAL_BG_COLOR;
    ctx.fillRect(
      0, overlay.height - MENU_BAR_HEIGHT,
      overlay.width, MENU_BAR_HEIGHT
    );

    let y = overlay.height - MENU_BAR_HEIGHT;

    // -- DRAW PLAY/PAUSE BUTTON --
    ctx.drawImage(is_paused ? play_button : pause_button,
      GAP_SIZE, y + (MENU_BAR_HEIGHT - BUTTON_SIZE) / 2,
      BUTTON_SIZE, BUTTON_SIZE
    );

    // -- DRAW PROGRESS BAR --
    let position = media_player.get_position();

    ctx.fillStyle = PROGRESS_BAR_BG_COLOR;
    ctx.fillRect(
      PROGRESS_BAR_X, y + (MENU_BAR_HEIGHT - PROGRESS_BAR_HEIGHT) / 2,
      PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT
    );
    ctx.fillStyle = PROGRESS_BAR_COLOR;
    ctx.fillRect(
      PROGRESS_BAR_X, y + (MENU_BAR_HEIGHT - PROGRESS_BAR_HEIGHT) / 2,
      PROGRESS_BAR_WIDTH * position, PROGRESS_BAR_HEIGHT
    );

    // DRAW PROGRESS TEXT (eg: 12:31 / 15:00) --
    let time = media_player.get_time();
    let seconds = Math.trunc(time / 1000);
    let minutes = Math.trunc(seconds / 60);
    seconds = ('00'+seconds % 60).slice(-2);
    let max_time = media_player.get_length();
    let max_seconds = Math.trunc(max_time / 1000);
    let max_minutes = Math.trunc(max_seconds / 60);
    max_seconds = ('00' + (max_seconds % 60)).slice(-2);

    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    ctx.fillStyle = TEXT_COLOR;
    ctx.fillText(
      `${minutes}:${seconds} / ${max_minutes}:${max_seconds}`,
      PROGRESS_TEXT_X + PROGRESS_TEXT_WIDTH, y + MENU_BAR_HEIGHT / 2,
    );

    // -- DRAW VOLUME/MUTE BUTTON --
    let is_muted = media_player.get_mute();
    ctx.drawImage(is_muted ? muted_button : volume_button,
      VOLUME_BUTTON_X, y + (MENU_BAR_HEIGHT - BUTTON_SIZE) / 2,
      BUTTON_SIZE, BUTTON_SIZE
    );

    // -- DRAW VOLUME BAR --
    let volume = media_player.get_volume() / 100;

    ctx.fillStyle = VOLUME_BAR_BG_COLOR;
    ctx.fillRect(
      VOLUME_BAR_X, y + (MENU_BAR_HEIGHT - VOLUME_BAR_HEIGHT) / 2,
      VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT
    );
    ctx.fillStyle = is_muted ? VOLUME_BAR_MUTE_COLOR : VOLUME_BAR_COLOR;
    ctx.fillRect(
      VOLUME_BAR_X, y + (MENU_BAR_HEIGHT - VOLUME_BAR_HEIGHT) / 2,
      VOLUME_BAR_WIDTH * volume, VOLUME_BAR_HEIGHT
    );
  }

  ctx.restore();
}

export function on_overlay_click(overlay, mouse_event) {
  const ctx = overlay.getContext("2d");
  const media_player = window.media_player;

  if (media_player == null) {
    // Video isn't loaded yet
    return;
  }

  const {
    VOLUME_BAR_WIDTH,
    VOLUME_BAR_HEIGHT,
    PROGRESS_BAR_WIDTH,
    PROGRESS_BAR_HEIGHT,
    PLAY_BUTTON_X,
    PROGRESS_BAR_X,
    VOLUME_BUTTON_X,
    VOLUME_BAR_X,
  } = get_layout(overlay);

  let canvas_rect = mouse_event.target.getBoundingClientRect();
  let x = mouse_event.clientX - canvas_rect.left;
  let y = mouse_event.clientY - canvas_rect.top;

  // User clicked in menu bar
  if (y > overlay.height - MENU_BAR_HEIGHT) {
    // User clicked on play/pause button
    if (x > PLAY_BUTTON_X && x < PLAY_BUTTON_X + BUTTON_SIZE) {
      media_player.toggle_play();
      update_overlay(overlay);
    }

    // User clicked on progress bar
    if (x > PROGRESS_BAR_X && x < PROGRESS_BAR_X + PROGRESS_BAR_WIDTH) {
      let progress = (x - PROGRESS_BAR_X) / PROGRESS_BAR_WIDTH;
      media_player.set_position(progress);
      update_overlay(overlay);
    }

    // User clicked on volume/mute button
    if (x > VOLUME_BUTTON_X && x < VOLUME_BUTTON_X + BUTTON_SIZE) {
      media_player.toggle_mute();
      update_overlay(overlay);
    }

    // User clicked on volume bar
    if (x > VOLUME_BAR_X && x < VOLUME_BAR_X + VOLUME_BAR_WIDTH) {
      let new_volume = (x - VOLUME_BAR_X) / VOLUME_BAR_WIDTH;
      media_player.set_volume(new_volume * 100);
      // Unmute
      media_player.set_mute(0);
      update_overlay(overlay);
    }
  }

  // User clicked outside the menu bar
    else {
      if (filepicker_button.files.length === 0) {
	  filepicker_button.click();
      }
      else {
	  media_player.toggle_play();
      }
  }
}
