RAIZ=$(git rev-parse --show-toplevel)

function baixar {
    if [[ -z "$1" ]]; then
        echo "Falta qual lab"
        return 1
    fi

    if [[ -z "$2" ]]; then
        fim=5
    else
        fim=$2
    fi

    mkdir Lab$1/in
    mkdir Lab$1/res

    i=1
    while [[ i -le fim ]]; do
        curl -k https://susy.ic.unicamp.br:9999/mc202abc/$1/dados/arq0$i.in -o $RAIZ/Lab$1/in/arq0$i.in
        curl -k https://susy.ic.unicamp.br:9999/mc202abc/$1/dados/arq0$i.res -o $RAIZ/Lab$1/res/arq0$i.res

        let i=i+1
    done
}

function testar {
    if [[ -z "$1" ]]; then
        echo "Falta qual lab"
        return 1
    fi
    if [[ -z "$2" ]]; then
        echo "Falta qual arquivo"
        return 2
    fi

    if [[ -z "$3" ]]; then
        fim=5
    else
        fim=$3
    fi

    i=1
    while [[ i -le fim ]]; do
        $RAIZ/Lab$1/$2 < $RAIZ/Lab$1/in/arq0$i.in > tmp
        diff tmp $RAIZ/Lab$1/res/arq0$i.res > /dev/null
        echo "Teste $i: $?"

        let i=i+1
    done

    rm tmp
}

function memoria {
    if [[ -z "$1" ]]; then
        echo "Falta qual lab"
        return 1
    fi
    if [[ -z "$2" ]]; then
        echo "Falta qual arquivo"
        return 2
    fi

    if [[ -z "$3" ]]; then
        fim=5
    else
        fim=$3
    fi

    i=1
    while [[ i -le fim ]]; do
        valgrind $RAIZ/Lab$1/$2 < $RAIZ/Lab$1/in/arq0$i.in > /dev/null

        let i=i+1
    done
}

$1 $2 $3 $4