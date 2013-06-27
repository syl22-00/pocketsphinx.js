
function wrapModule() {
    return {psGetState: Module.cwrap('psGetState'),
            psGetHyp: Module.cwrap('psGetHyp', 'string'),
            psInitialize: Module.cwrap('psInitialize'),
            psStartGrammar: Module.cwrap('psStartGrammar', 'number', ['number']),
            psEndGrammar: Module.cwrap('psEndGrammar', 'number', ['number']),
            psSwitchGrammar: Module.cwrap('psSwitchGrammar', 'number', ['number']),
            psAddWord: Module.cwrap('psAddWord', 'number', ['number','number']),
            psAddTransition: Module.cwrap('psAddTransition', 'number', ['number','number','number']),
            psStart: Module.cwrap('psStart'),
            psStop: Module.cwrap('psStop'),
            psProcess: Module.cwrap('psProcess', 'number', ['number','number']),
            c_malloc: Module.cwrap('malloc', 'number', ['number']),
            c_free: Module.cwrap('free', 'number', ['number'])};
}
