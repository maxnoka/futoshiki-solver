<?php
require_once('Row.php');
require_once('Constraint.php');

class Game {
    public $board_size;
    public $rows = array();
    public $constraints = array("row" => array(), "col" => array());

    public function __construct($board_size) {
        // NxN array filled with zeros
        $this->board_size = $board_size;

        for ($i=0; $i < $board_size; $i++) { 
            $this->rows[] = new Row($board_size, $i);
        }

        for ($rowIdx=0; $rowIdx < $board_size; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size - 1; $colIdx++) {
                $new = new Constraint($colIdx, $rowIdx, "+", "row");
                $this->constraints["row"][$rowIdx][] = $new;
            }
        }

        for ($rowIdx=0; $rowIdx < $board_size - 1; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size; $colIdx++) {    
                $new = new Constraint($colIdx, $rowIdx, "+", "col");
                $this->constraints["col"][$colIdx][] = $new;
            }
        }
    }

    public static function createFromJson($json) {
        $data = json_decode($json);

        if ($data === null) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid json."));
            exit();
        }

        if(!isset($data->board_size) ||
           !isset($data->rows) ||
           !isset($data->constraints)){
            // set response code - 400 bad request
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Data is incomplete."));
            exit();
        }

        $size = filter_var(htmlspecialchars($data->board_size), FILTER_VALIDATE_INT);
        if ($size === false) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Board size is invalid (require int)."));
            exit();
        }

        if ($size > 7 || $size < 2) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Board size is invalid (needs to be in range [2,7])."));
            exit();
        }

        $obj = new Game($size);

        if (gettype($data->rows) != "array") {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (rows)"));
            exit();
        }
        
        if (sizeof($data->rows) != $size) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (needs to be NxN shape, where N=board_size)"));
            exit();
        }

        foreach ($data->rows as $rowIdx => $row) {
            if (gettype($row) != "array") {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (row within rows)"));
                exit();
            }

            if (sizeof($row) != $size) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (needs to be NxN shape)"));
                exit();
            } 

            foreach ($row as $cellIdx => $val) {
                $validVal = filter_var(htmlspecialchars($val), FILTER_VALIDATE_INT);
                if ($validVal === false) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Board value is invalid (require int)."));
                    exit();
                }

                if ($validVal > $size) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Board value is invalid (must be less than the size of the board)."));
                    exit();
                }
                $obj->rows[$rowIdx]->cells[$cellIdx]->val = $validVal;
            }
        }

        if (gettype($data->constraints) != "array") {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (constraints)"));
            exit();
        }

        foreach ($data->constraints as $constraintIdx => $constraintJson) {
            if(!isset($constraintJson->colIdx) ||
               !isset($constraintJson->rowIdx) ||
               !isset($constraintJson->operator) ||
               !isset($constraintJson->type)){
                // set response code - 400 bad request 
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Data is incomplete. (constraints)"));
                exit();
            }

            $validColIdx = filter_var(htmlspecialchars($constraintJson->colIdx), FILTER_VALIDATE_INT);
            $validRowIdx = filter_var(htmlspecialchars($constraintJson->rowIdx), FILTER_VALIDATE_INT);

            if ($constraintJson->operator == ">") {$validOperator = ">";}
            else if ($constraintJson->operator == "<") {$validOperator = "<";}
            else if ($constraintJson->operator == "+") {$validOperator = "+";}
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint operator is invalid."));
                exit();
            }

            if ($constraintJson->type == "row") {$validType = "row";}
            else if ($constraintJson->type == "col") {$validType = "col";}
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint type is invalid."));
                exit();
            }

            $eitherNegatve = $validColIdx < 0 || $validRowIdx < 0;
            $eitherTooLarge = false;
            if (($validType == "row")) {
                if ($validColIdx >= $size - 1 || $validRowIdx >= $size) {
                    $eitherTooLarge = true;
                }
            }
            else {
                if ($validColIdx >= $size || $validRowIdx >= $size - 1) {
                    $eitherTooLarge = true;
                }
            }

            if ($eitherNegatve || $eitherTooLarge) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint indices invalid."));
                exit();
            }

            $obj->addConstraint($validColIdx, $validRowIdx, $validOperator, $validType);
        }
        return $obj;
    }

    public function render() {
        include TEMPLATE_PATH . '/' . 'game.html';
    }

    public function solve() {
        $command = "./futoshiki-solver ";
        $command .= "\"" . $this->serializeBoard() . "\" ";
        $command .= "\"" . $this->serializeConstraints() . "\"";

        $output=null;
        $retval=null;

        exec($command, $output, $retval);

        if ($retval == 2) {
            return "not valid";
        }

        $finalOutput = array();
        foreach ($output as $outRowIdx => $outRow) {
            $finalOutput[] = explode(",", $outRow);

        }
        return $finalOutput;
    }

    public function addConstraint($colIdx, $rowIdx, $operator, $type) {
        if ($type == "row") {
            $this->constraints["row"][$rowIdx][$colIdx]->operator = $operator;
        }
        else {
            $this->constraints["col"][$colIdx][$rowIdx]->operator = $operator;
        }
    }

    public function serializeBoard() {
        $rowStrings = array();
        foreach ($this->rows as $rowIdx => $row) {
            $rowStrings[] = $row->serializeRow();
        }
        return implode("\n", $rowStrings);
    }

    public function serializeConstraints() {
        $constraintStrings = array();
        foreach ($this->constraints as $i => $constraintArray) {
            foreach ($constraintArray as $rowIdx => $constraintRow) {
                foreach ($constraintRow as $constraintIdx => $constraint) {
                    if ($constraint->operator != "+") {
                        $constraintStrings[] = $constraint->serializeConstraint();
                }
            }
            }
        }
        return implode("\n", $constraintStrings);
    }
}
?>