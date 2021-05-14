<?php
require_once(__DIR__ . "./../config/config.php");

require_once('Row.php');
require_once('Constraint.php');

class Game {
    public $board_size;
    public $rows = array();
    public $constraints = array("row" => array(), "col" => array());

    public function __construct($board_size) {
        $this->board_size = $board_size;

        // have $board_size # of rows
        // comprised of $board_size # of cols
        // all set to be empty (zero)
        for ($i=0; $i < $board_size; $i++) { 
            $this->rows[] = new Row($board_size, $i);
        }
        
        // create a constraint of each set of adjacent cells
        // all set with constraint type "+" (no constraint)  

        // the constraints between cells of the same row
        for ($rowIdx=0; $rowIdx < $board_size; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size - 1; $colIdx++) {
                $new = new Constraint($colIdx, $rowIdx, "+", "row");
                $this->constraints["row"][$rowIdx][] = $new;
            }
        }

        // the constraints between cells of different rows
        for ($rowIdx=0; $rowIdx < $board_size; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size - 1; $colIdx++) {    
                $new = new Constraint($colIdx, $rowIdx, "+", "col");
                $this->constraints["col"][$colIdx][] = $new;
            }
        }
    }

    public function render() {
        include TEMPLATE_PATH . '/' . 'game.html';
    }
}
?>
