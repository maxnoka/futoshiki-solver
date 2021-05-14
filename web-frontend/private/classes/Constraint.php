<?php
require_once(__DIR__ . "./../config/config.php");

class Constraint {
    // object properties
    public $colIdx;
    public $rowIdx;
    public $operator;

    public $type; // "row" or "col"
    // explanation:
    //	+-----------+
    //  | 1 | &	| 2	|
    //  +---+---+---+
    //  | *	|	| *	|
    //  +---+---+---+
    //  | 2	| &	|	|
    //	+-----------+
    //  we think of the * type as a "col" type, & as "row"
    //  then the board can have (n-1)*n of each type


    public function __construct($colIdx, $rowIdx, $operator, $type) {
        $this->colIdx = $colIdx;
        $this->rowIdx = $rowIdx;
        $this->operator = $operator;
        $this->type = $type;
    }

    public function render() {
        if ($this->type == "row") {
            include TEMPLATE_PATH . '/' . 'constraint-row.html';
        }
        else {
            include TEMPLATE_PATH . '/' . 'constraint-col.html';
        }
    }
}
?>
