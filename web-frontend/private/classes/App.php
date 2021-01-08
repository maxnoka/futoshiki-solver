<?php
require_once(__DIR__ . "/../config/config.php");
require_once('Game.php');

class App {
    protected $stylesheets = array("style.css", "arrow.css");
    protected $scripts = array("main.js");
    protected $title = "Futoshiki Solver";
    
    protected $game;

    public function __construct($boardSize = 4) {
        $this->game  = new Game($boardSize);
    }

    public function render() {
        include TEMPLATE_PATH . '/' . 'base-template.html';
    }
}
?>
