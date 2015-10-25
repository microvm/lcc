{
"f:hello": {
"sym": "s:g:hello",
"caller": [ "s:pr:(hello) name" ],
"callee": [ "s:p:(hello) name" ],
"body": [ "n:1", "n:2" ]
},
"n:1": {
"op": "ASGN",
"optype": "P",
"opsize": 8,
"kid1": "n:3",
"kid2": "n:4",
"sym1": "s:c:8",
"sym2": "s:c:1",
"done": 1},
"s:p:(hello) name": {
"name": "name",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:*char",
"done": 1}
,
"t:*char": {
"op": "POINTER",
"type": "t:char",
"align": 1,
"size": 8,
"done": 1},
"t:char": {
"op": "INT",
"align": 1,
"size": 1,
"done": 1},
"s:pr:(hello) name": {
"name": "name",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:*char",
"done": 1}
,
"s:0:(hello) h": {
"name": "h",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:*char",
"done": 1}
,
"s:l:1": {
"label": 1
}
,
"s:c:1": {
"sclass": "STATIC",
"const": {
"type": "t:int",
"v": 1,
"done": 1}
}
,
"t:int": {
"op": "INT",
"align": 1,
"size": 4,
"done": 1},
"s:c:8": {
"sclass": "STATIC",
"const": {
"type": "t:int",
"v": 8,
"done": 1}
}
,
"s:g:2": {
"name": "2",
"sclass": "STATIC",
"addressed": false,
"computed": false,
"temporary": false,
"generated": true,
"defined": false,
"type": "t:[6char",
"done": 1}
,
"t:[6char": {
"op": "ARRAY",
"type": "t:char",
"align": 1,
"size": 6,
"done": 1},
"s:g:hello": {
"name": "hello",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:type0",
"defined_in": "tst\timm.c",
"done": 1}
,
"t:type0": {
"op": "FUNCTION",
"type": "t:void",
"align": 0,
"size": 0,
"oldstyle": false,
"proto": [ "t:*char" ]
"done": 1},
"t:void": {
"op": "VOID",
"align": 0,
"size": 0,
"done": 1},
"n:4": {
"op": "ADDRG",
"optype": "P",
"opsize": 8,
"sym1": "s:g:2",
"done": 1},
"n:3": {
"op": "ADDRL",
"optype": "P",
"opsize": 8,
"sym1": "s:0:(hello) h",
"done": 1},
"n:2": {
"op": "LABEL",
"optype": "V",
"opsize": 0,
"sym1": "s:l:1",
"done": 1},
"f:main": {
"sym": "s:g:main",
"caller": [ "s:pr:(main) argc", "s:pr:(main) argv" ],
"callee": [ "s:p:(main) argc", "s:p:(main) argv" ],
"body": [ "n:5", "n:6", "n:7", "n:8", "n:9", "n:10", "n:11", "n:12" ]
},
"n:5": {
"op": "GE",
"optype": "I",
"opsize": 4,
"kid1": "n:13",
"kid2": "n:14",
"sym1": "s:l:4",
"done": 1},
"s:p:(main) argv": {
"name": "argv",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:**char",
"done": 1}
,
"t:**char": {
"op": "POINTER",
"type": "t:*char",
"align": 1,
"size": 8,
"done": 1},
"t:*char": {
"op": "POINTER",
"type": "t:char",
"align": 1,
"size": 8,
"done": 1},
"s:p:(main) argc": {
"name": "argc",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:int",
"done": 1}
,
"s:pr:(main) argv": {
"name": "argv",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:**char",
"done": 1}
,
"s:pr:(main) argc": {
"name": "argc",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:int",
"done": 1}
,
"s:c:0": {
"sclass": "STATIC",
"const": {
"type": "t:int",
"v": 0,
"done": 1}
}
,
"s:c:8": {
"sclass": "STATIC",
"const": {
"type": "t:long int",
"v": 8,
"done": 1}
}
,
"t:long int": {
"op": "INT",
"align": 1,
"size": 8,
"done": 1},
"s:l:3": {
"label": 3
}
,
"s:l:4": {
"label": 4
}
,
"s:c:2": {
"sclass": "STATIC",
"const": {
"type": "t:int",
"v": 2,
"done": 1}
}
,
"s:g:main": {
"name": "main",
"sclass": "AUTO",
"addressed": false,
"computed": false,
"temporary": false,
"generated": false,
"defined": true,
"type": "t:type1",
"defined_in": "tst\timm.c",
"done": 1}
,
"t:type1": {
"op": "FUNCTION",
"type": "t:int",
"align": 0,
"size": 0,
"oldstyle": false,
"proto": [ "t:int", "t:**char" ]
"done": 1},
"n:14": {
"op": "CNST",
"optype": "I",
"opsize": 4,
"sym1": "s:c:2",
"done": 1},
"n:13": {
"op": "INDIR",
"optype": "I",
"opsize": 4,
"kid1": "n:15",
"done": 1},
"n:15": {
"op": "ADDRF",
"optype": "P",
"opsize": 8,
"sym1": "s:p:(main) argc",
"done": 1},
"n:6": {
"op": "RET",
"optype": "I",
"opsize": 4,
"kid1": "n:16",
"done": 1},
"n:16": {
"op": "CNST",
"optype": "I",
"opsize": 4,
"sym1": "s:c:1",
"done": 1},
"n:7": {
"op": "JUMP",
"optype": "V",
"opsize": 0,
"kid1": "n:17",
"done": 1},
"n:17": {
"op": "ADDRG",
"optype": "P",
"opsize": 8,
"sym1": "s:l:3",
"done": 1},
"n:8": {
"op": "LABEL",
"optype": "V",
"opsize": 0,
"sym1": "s:l:4",
"done": 1},
"n:10": {
"op": "CALL",
"optype": "V",
"opsize": 0,
"kid1": "n:18",
"type": "t:type0",
"done": 1},
"n:18": {
"op": "ADDRG",
"optype": "P",
"opsize": 8,
"sym1": "s:g:hello",
"done": 1},
"n:9": {
"op": "ARG",
"optype": "P",
"opsize": 8,
"kid1": "n:19",
"sym1": "s:c:8",
"sym2": "s:c:1",
"done": 1},
"n:19": {
"op": "INDIR",
"optype": "P",
"opsize": 8,
"kid1": "n:20",
"done": 1},
"n:20": {
"op": "ADD",
"optype": "P",
"opsize": 8,
"kid1": "n:21",
"kid2": "n:22",
"done": 1},
"n:22": {
"op": "CNST",
"optype": "I",
"opsize": 8,
"sym1": "s:c:8",
"done": 1},
"n:21": {
"op": "INDIR",
"optype": "P",
"opsize": 8,
"kid1": "n:23",
"done": 1},
"n:23": {
"op": "ADDRF",
"optype": "P",
"opsize": 8,
"sym1": "s:p:(main) argv",
"done": 1},
"n:11": {
"op": "RET",
"optype": "I",
"opsize": 4,
"kid1": "n:24",
"done": 1},
"n:24": {
"op": "CNST",
"optype": "I",
"opsize": 4,
"sym1": "s:c:0",
"done": 1},
"n:12": {
"op": "LABEL",
"optype": "V",
"opsize": 0,
"sym1": "s:l:3",
"done": 1},
"b:0": {
"symbol": "s:g:2",
"contents": [ 104, 101, 108, 108, 111, 0 ]},
"done": 1}
