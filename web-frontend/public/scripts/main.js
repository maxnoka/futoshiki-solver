$(document).ready(function(){
  initConstraintButtons();

  var clearButton = document.querySelector('#puzzle-clear-button');
  clearButton.onclick = function () {
    clearError();
    clearBoard();
    initConstraintButtons();
    return false; // don't reload
  };
});

$(document).on('click', '#puzzle-generate-button', function(){
  clearError();
  clearBoard();
  disableConstraintButtons();

  var size = document.querySelectorAll('[class^="puzzle-row-numbers"]').length;
  var toSend = {"type" : "futoshiki",
                "size": size};

  $.ajax({
    type: "POST",
    url: "http://localhost:18080/generate",
    data: JSON.stringify(toSend),
    success: function(data){
      fillBoard(data.cells, true);
      fillConstraints(data.constraints);
    },
    error: function(data) {
      displayError("Invalid input");
    }
  });

  return false;
});

$(document).on('click', '#puzzle-solve-button', function(){
  clearError();
  setSetCellsToStarting();
  disableConstraintButtons();

  var toSend = {"grid_size" : 0,
                "cells" : [],
                "num_cells" : 4,
                "constraints": []};

  toSend.cells = getCellVals();
  toSend.grid_size = toSend.cells.length;
  toSend.num_cells = toSend.grid_size * toSend.grid_size;
  toSend.constraints = getConstraints();

  $.ajax({
    type: "POST",
    url: "http://localhost:18080/solve",
    data: JSON.stringify(toSend),
    success: function(data){
      if(data.solved) {
        fillBoard(data.details.solutions[0].cells, false);
      }
      else {
        displayError(data.outcome);
      }
    },
    error: function(data) {
      displayError("Invalid input");
    }
  });

  return false;
});

function clearError() {
    var statusBar = document.getElementById("puzzle-status-bar");
    statusBar.innerText = "";
}

function displayError(outcomeString) {
    var statusBar = document.getElementById("puzzle-status-bar");
    statusBar.innerText = outcomeString;
}


function initConstraintButtons() {
  var buttons = document.querySelectorAll('[id^="constraint-button-"]');

  buttons.forEach(puzzle_button => {
    var idString = puzzle_button.id.substring(18);
    var show = document.getElementById("constraint-display-" + idString);

    puzzle_button.onclick = function () {
      // cyclical order: < , > , [blank]
      if (show.innerText == "<") {
        show.innerText = ">";
      }
      else if (show.innerText == ">") {
        show.innerText = " ";
      }
      else {
        show.innerText = "<";
      }
    }
  });
};

function disableConstraintButtons() {
  var buttons = document.querySelectorAll('[id^="constraint-button-"]');

  buttons.forEach(puzzle_button => {
    puzzle_button.onclick = function () { }
  });

}

function setSetCellsToStarting() {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');
  rows.forEach((row) => {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    cells.forEach((cell) => {
      var element = cell.querySelector('input');
      var val = element.value;
      if (val != "") {
        element.classList.add("puzzle-cell-starting");
        element.setAttribute("disabled", "");
      }
    });
  });
}

function getCellVals() {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');
  var rowsVals = []

  var numRows = rows.length;
  var possibleVals = Array.from(new Array(numRows), (x, i) => i + 1);

  Object.entries(rows).forEach(([rowIdx, row]) => {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    var cellsVals = [];
    Object.entries(cells).forEach(([colIdx, cell]) => {
      var val = cell.querySelector('input').value;
      var out = { 
        "cell_id": `${colIdx}_${rowIdx}`,
        "possible_vals": possibleVals
      };
      if (val == "") {
        out["val"] = 0;
      }
      else {
        out["val"] = Number(val);
      }
      cellsVals.push(out);
    });
    rowsVals.push(cellsVals);
  });
  return rowsVals;
}

function fillConstraints(data) {
  data.forEach(constraint => {
    var lhsCellId = constraint.cells[0];
    var rhsCellId = constraint.cells[1];

    var lhsColIdx = Number(lhsCellId[0]);
    var lhsRowIdx = Number(lhsCellId[2]);

    var rhsColIdx = Number(rhsCellId[0]);
    var rhsRowIdx = Number(rhsCellId[2]);

    var diffX = lhsColIdx - rhsColIdx;
    var diffY = lhsRowIdx - rhsRowIdx;

    var type = "";
    var constraintColIdx = 0;
    var constraintRowIdx = 0;
    if (Math.abs(diffX) == 1 && diffY == 0) {
        type = "row";
        constraintColIdx = rhsColIdx + diffX;
        constraintRowIdx = lhsRowIdx;
    }
    else if (Math.abs(diffY) == 1 && diffX == 0) {
        type = "col";
        constraintColIdx = lhsColIdx;
        constraintRowIdx = rhsRowIdx + diffY;
    }

    var operator = "";
    if (constraint.operator == ">") {
        if (diffX < 0 || diffY < 0) {
            operator = "<";
        }
        operator = ">";
    }
    else if (constraint.operator == "<") {
        if (diffX < 0 || diffY < 0) {
            operator = ">";
        }
        operator = "<";
    }

    if (type == "row") {
        var element = document.querySelector(`#constraint-display-${type}_${constraintRowIdx}_${constraintColIdx}`)
    }
    else {
        var element = document.querySelector(`#constraint-display-${type}_${constraintColIdx}_${constraintRowIdx}`)
    }
    element.innerText = operator;
  });
}

function getConstraints() {
  var constraints = document.querySelectorAll('[id^="constraint-display-"]');
  var out = [];
  var constraintCounter = 0;

  constraints.forEach(constraint => {
    var idString = constraint.id.substring(19);  // leaves e.g. row_0_0
    var myRe = /(?<=_)[\d]+/gm;  // gets the digits after each underscore
    var matches = idString.match(myRe);

    var singleOut = {"cells" : [],
                     "type": "inequality",
                     "constraint_id": `const_${constraintCounter}`,
                     "operator" : ""};

    var operator = constraint.innerText;
    if (operator == "<") {
      singleOut.operator = "<";
    }
    else if (operator == ">") {
      singleOut.operator = ">";
    }
    else {
      return;
    }

    var type = idString.substring(0, 3);
    if (type == "row") {
      rowIdx_old = Number(matches[0]);
      colIdx_old = Number(matches[1]);
      rowIdx_lhs = rowIdx_old;
      rowIdx_rhs = rowIdx_old;
      colIdx_lhs = colIdx_old;
      colIdx_rhs = colIdx_old + 1;
    }
    else if (type == "col") {
      rowIdx_old = Number(matches[1]);
      colIdx_old = Number(matches[0]);
      rowIdx_lhs = rowIdx_old;
      rowIdx_rhs = rowIdx_old + 1;
      colIdx_lhs = colIdx_old;
      colIdx_rhs = colIdx_old;
    }
    else {
      return;
    }

    singleOut.cells.push(`${colIdx_lhs}_${rowIdx_lhs}`);
    singleOut.cells.push(`${colIdx_rhs}_${rowIdx_rhs}`);
    out.push(singleOut);
  });

  return out;
}

function clearBoard() {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');

  rows.forEach(row => {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    cells.forEach(cell => {
      var element = cell.querySelector('input');
      element.value = null;
      element.removeAttribute("disabled", "");
    });
  }); 

  var constraints = document.querySelectorAll('.puzzle-cell-constraint');
  constraints.forEach(constraint => {
    constraint.innerText = " ";
  });
};

function fillBoard(data, setStarting) {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');

  rows.forEach(function(row, rowIndex) {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    cells.forEach(function(cell, cellIndex) {
      var val = data[rowIndex][cellIndex]["val"];
      if (val != 0) {
        var element = cell.querySelector('input');
        element.value = val;
        if (setStarting) {
            element.classList.add("puzzle-cell-starting");
            element.setAttribute("disabled", "");
        }
      }
    });
  });
};
