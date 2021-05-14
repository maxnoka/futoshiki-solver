<?php
require_once(__DIR__ . "./../config/config.php");

require_once('Cell.php');

class Row {
    public $cells = array();
    public $rowIdx;

    public function __construct($board_size, $rowIdx) {
        for ($i = 0; $i < $board_size; $i++) { 
        	$this->cells[] = new Cell(0, $rowIdx, $i);
        }
        $this->$rowIdx = $rowIdx;
    }

    public function render($rowConstraints) {
    	include TEMPLATE_PATH . '/' . 'row.html';
    }
}
?>