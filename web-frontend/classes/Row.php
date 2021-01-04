<?php

require_once('Cell.php');

class Row {
    // object properties
    public $cells = array();
    public $rowIdx;

    public function __construct($board_size, $rowIdx) {
        // NxN array filled with zeros
        for ($i=0; $i < $board_size; $i++) { 
        	$this->cells[] = new Cell(0, $rowIdx, $i);
        }
        $this->$rowIdx = $rowIdx;
    }

    public function serializeRow() {
        $vals = array();
        foreach ($this->cells as $cellIdx => $cell) {
            $vals[] = $cell->val;
        }
    	return implode(",", $vals);
    }

    public function render($rowConstraints) {
    	include TEMPLATE_PATH . '/' . 'row.html';
    }
}
?>