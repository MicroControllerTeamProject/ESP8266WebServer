const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="it" class="h-100">
<head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no" />

    <!-- Bootstrap CSS -->
    <!--<link rel="stylesheet" href="Content/bootstrap.min.css" crossorigin="anonymous">-->
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous" />
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-treeview/1.2.0/bootstrap-treeview.min.css" />
    <title></title>
</head>
<body class="h-100 bg-success">
    <div class="container h-100 bg-warning">
        <h1>Home controller</h1>
        <div class="row">
            <div id="treeview-expandible" class="col-12"></div>
        </div>
    </div>
</body>
</html>

<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-treeview/1.2.0/bootstrap-treeview.min.js"></script>

<script type="text/javascript">
    $(function () {
        var json = '[' +
            '{' +
            '"text": "KITCHEN",' +
            '"nodes": [' +
            '{' +
            '"text": "REQUESTS",' +
            '"nodes": [' +
            '{' +
            '"text": "FOOD",' +
            '"href": "http://google.com"' +
            '}' +
            ']' +
            '},' +
            '{' +
            '"text": "COMMANDS"' +
            '}' +
            ']' +
            '},' +
            '{' +
            '"text": "Parent 2"' +
            '},' +
            '{' +
            '"text": "Parent 3"' +
            '},' +
            '{' +
            '"text": "Parent 4"' +
            '},' +
            '{' +
            '"text": "Parent 5"' +
            '}' +
            ']';

        var nodeToSelected = '';

        var $expandibleTree = $('#treeview-expandible').treeview({
            data: json,
            enableLinks: true,
            levels: 1,

            onNodeSelected: function (event, data) {
                nodeToSelected = data.text;
                //alert(nodeToSelected);
                expandibleNodes = findExpandibleNodess();

                $('.expand-node').prop('disabled', !(expandibleNodes.length >= 1));


                if (!data.state.expanded) {

                    var levels = $('#select-expand-node-levels').val();
                    $expandibleTree.treeview('expandNode', [expandibleNodes, { levels: levels, silent: $('#chk-expand-silent').is(':checked') }]);
                }
                else {
                    $expandibleTree.treeview('collapseNode', [expandibleNodes, { silent: $('#chk-expand-silent').is(':checked') }]);
                }

            },


            onNodeCollapsed: function (event, node) {
                $('#expandible-output').prepend('<p>' + node.text + ' was collapsed</p>');
            },
            onNodeExpanded: function (event, node) {
                $('#expandible-output').prepend('<p>' + node.text + ' was expanded</p>');
            }
        });

        //var findExpandibleNodess = function() {
        // return $expandibleTree.treeview('search', [ $('#input-expand-node').val(), { ignoreCase: false, exactMatch: false } ]);
        //};

        var findExpandibleNodess = function () {
            //alert(nodeToSelected);
            return $expandibleTree.treeview('search', [nodeToSelected, { ignoreCase: false, exactMatch: false }]);
        };

        var expandibleNodes = findExpandibleNodess();

        // Expand/collapse/toggle nodes
        $('#input-expand-node').on('keyup', function (e) {
            expandibleNodes = findExpandibleNodess();
            $('.expand-node').prop('disabled', !(expandibleNodes.length >= 1));
        });

        $('#btn-expand-node.expand-node').on('click', function (e) {
            var levels = $('#select-expand-node-levels').val();
            $expandibleTree.treeview('expandNode', [expandibleNodes, { levels: levels, silent: $('#chk-expand-silent').is(':checked') }]);
        });

        $('#btn-collapse-node.expand-node').on('click', function (e) {
            $expandibleTree.treeview('collapseNode', [expandibleNodes, { silent: $('#chk-expand-silent').is(':checked') }]);
        });

        $('#btn-toggle-expanded.expand-node').on('click', function (e) {
            $expandibleTree.treeview('toggleNodeExpanded', [expandibleNodes, { silent: $('#chk-expand-silent').is(':checked') }]);
        });



        $('#btn-collapse-all').on('click', function (e) {
            $expandibleTree.treeview('collapseAll', { silent: $('#chk-expand-silent').is(':checked') });
        });
    });  
</script>
)=====";