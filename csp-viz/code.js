function GenPossibleOptionsTable(possibleOptionsArray) {
    var html = '<table class="possible-vals-table">';
    for (var i = 1; i <= 9; i++) {
        if ( (i - 1) % 3 == 0) {
            html += "<tr>";
        }

        html += '<td><div class="possible-val-cell">'
        if (possibleOptionsArray.includes(i)) {
            html += i;
        }
       html += "</div></td>";

        if ( i % 3 == 0) {
            html += "</tr>";
        }
    }
    html += "</table>";
    return html;
}


fetch('http://localhost:18080')
    .then(response => response.json())
    .then(inData => {
        var outdata = {
            nodes: [], 
            edges: []
        };

        for ( var i = 0; i < inData.cells.length; i+=1 ) {
            outdata.nodes.push( { 
                data: { 
                    id: 'cell_' + inData.cells[i].cell_id ,
                    val: inData.cells[i].val,
                    possible_vals: inData.cells[i].possible_vals
                },
                classes: 'cell' 
            } );
        }

        for ( var i = 0; i < inData.constraints.length; i+=1 ) {
            outdata.nodes.push( { 
                data: { 
                    id: 'cnst' + inData.constraints[i].constraint_id,
                    operator: inData.constraints[i].operator
                },
                classes: 'constraint'
            });

            if (inData.constraints[i].type == "inequality") {
                // from lhs to constraint
                outdata.edges.push( { 
                    data: {  
                        id: 'link_' + inData.constraints[i].constraint_id + '_' + inData.constraints[i].cells[0],
                        operator: inData.constraints[i].operator,
                        source: 'cell_' + inData.constraints[i].cells[0],
                        target: 'cnst' + inData.constraints[i].constraint_id
                    } 
                });

                // from constraint to rhs
                outdata.edges.push( { 
                    data: { 
                        id: 'link_' + inData.constraints[i].constraint_id + '_' + inData.constraints[i].cells[1],
                        operator: inData.constraints[i].operator,
                        source: 'cnst' + inData.constraints[i].constraint_id,
                        target: 'cell_' + inData.constraints[i].cells[1]
                    } 
                });
            }

            else if (inData.constraints[i].type == "equality") {
                for (var j = 0; j < inData.constraints[i].cells.length; j+=1 ) {
                    outdata.edges.push( { 
                        data: { 
                            id: 'link_' + inData.constraints[i].constraint_id + '_' + inData.constraints[i].cells[j],
                            operator: inData.constraints[i].operator,
                            source: 'cnst' + inData.constraints[i].constraint_id,
                            target: 'cell_' + inData.constraints[i].cells[j]
                        } 
                    });
                }
            }
        }

        return outdata;
    })
    .then(function(inData) {
        var cy = window.cy = cytoscape({
          container: document.getElementById('cy'),

          boxSelectionEnabled: false,

          style: [
            {
              selector: ':parent',
              css: {
                'text-valign': 'top',
                'text-halign': 'center',
              }
            },
            {
              selector: '.cell',
              css: {
                'shape': 'square',
                'background-color': 'cornflowerblue',
                'width': 40,
                'height': 40
              }
            },
            {
              selector: '.constraint',
              css: {
                'shape': 'diamond',
                'background-color': 'burlywood'
              }
            },
            {
              selector: 'edge',
              css: {
                'curve-style': 'bezier',
                'target-arrow-shape': 'triangle'
              }
            }
          ],

          elements: {
            nodes: inData.nodes,
            edges: inData.edges
          },

          layout: {
            name: 'cose'
          }
        });

        cy.nodeHtmlLabel([
        {
            query: '.cell',
            valign: 'bottom',
            valignBox: 'bottom',
            tpl: function(data) {
                return '<div class="cy-title__p1"> ' + data.id + '</div>';
            }
        }
        ]);
        cy.nodeHtmlLabel([
        {
            query: '.cell',
            valign: 'center',
            tpl: function(data) {
                return '<div class="cy-title__p2">' + GenPossibleOptionsTable(data.possible_vals) + '</div>';
            }
        }
        ]);

        cy.nodeHtmlLabel([
        {
            query: '.constraint',
            valign: 'bottom',
            tpl: function(data) {
                return '<p class="cy-title__p3"> ' + data.id + '</p>';
            }
        }
        ]);
        cy.nodeHtmlLabel([
        {
            query: '.constraint',
            valign: 'center',
            tpl: function(data) {
                return '<p class="cy-title__p4"> ' + data.operator + '</p>';
            }
        }
        ]);
    });
