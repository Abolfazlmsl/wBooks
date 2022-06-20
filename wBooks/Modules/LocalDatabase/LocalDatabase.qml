import QtQuick 2.12
import QtQuick.LocalStorage 2.0

Item {
    // reference to the database object
    property var db;

    //-- create database --//
    function initDatabase() {
        console.log('initDatabase()')
        db = LocalStorage.openDatabaseSync("CrazyBox", "1.0", "A box who remembers its position", 100000);
        db.transaction( function(tx) {
            print('... create table')
            tx.executeSql('CREATE TABLE IF NOT EXISTS books(id INTEGER, VALUE TEXT, image TEXT)');
        });

    }

    //-- add Table database --//
    function addTable(table, image=false) {
        db.transaction( function(tx) {
            if (image){
                tx.executeSql('CREATE TABLE IF NOT EXISTS '+ table +'(id INTEGER, VALUE TEXT, image TEXT)');
            }else{
                tx.executeSql('CREATE TABLE IF NOT EXISTS '+ table +'(id INTEGER, VALUE TEXT)');
            }
        });
    }

    //-- write data to database (data has an id) --//
    function storeData(table, data, dir, callBack) {

        if(!db) { return; }
        db.transaction( function(tx) {
            if (data.id){
                var result = tx.executeSql('SELECT * from '+ table +' where id = (?)', data.id);
            }else{
                var result = tx.executeSql('SELECT * from '+ table +' where id = (?)', 1);
            }
            if(result.rows.length === 1) {// use update
                if (data.id){
                    if (dir){
                        result = tx.executeSql('UPDATE '+ table +' set value=?, image=? where id=(?)', [JSON.stringify(data), dir, data.id]);
                    }else{
                        result = tx.executeSql('UPDATE '+ table +' set value=? where id=(?)', [JSON.stringify(data), data.id]);
                    }
                }else{
                    result = tx.executeSql('UPDATE '+ table +' set value=? where id=(?)', [JSON.stringify(data), 1]);
                }
            } else { // use insert

                if (data.id){
                    if (dir || dir===""){
                        result = tx.executeSql('INSERT INTO '+ table +' VALUES (?,?,?)', [data.id, JSON.stringify(data), dir]);
                    }else{
                        result = tx.executeSql('INSERT INTO '+ table +' VALUES (?,?)', [data.id, JSON.stringify(data)]);
                    }
                }else{
                    if (dir || dir===""){
                        result = tx.executeSql('INSERT INTO '+ table +' VALUES (?,?,?)', [1, JSON.stringify(data), dir]);
                    }else{
                        result = tx.executeSql('INSERT INTO '+ table +' VALUES (?,?)', [1, JSON.stringify(data)]);
                    }
                }
            }

            if(callBack){
                callBack()
            }
        });
    }

    //-- Add data to database (data dosnt have any id) --//
    function addData(table, data, callBack) {

        var array = []
        if(!db) { return; }
        db.transaction( function(tx) {

            var result = tx.executeSql('SELECT id from '+ table);
            for (var i=0; i<result.rows.length; i++){
                array.push(JSON.parse(JSON.stringify(result.rows.item(i))).id)
            }
            var data_id = generateRandom(array)

            data["id"] = data_id
            result = tx.executeSql('INSERT INTO '+ table +' VALUES (?,?)', [data_id, JSON.stringify(data)]);

            if(callBack){
                callBack()
            }
        });
    }


    //-- delete Table from database --//
    function deleteTable(table, callBack) {
        if(!db) { return; }
        db.transaction( function(tx) {
            var result = tx.executeSql('DROP TABLE '+ table);
            if(result.rowsAffected > 0) {
                if(callBack){
                    callBack()
                }
            }
        });
    }

    //-- delete data from database --//
    function deleteData(table, id, callBack) {
        if(!db) { return; }
        db.transaction( function(tx) {
            var result = tx.executeSql('DELETE FROM '+ table +' WHERE id=(?)', [id]);
            if(result.rowsAffected > 0) {
                if(callBack){
                    callBack()
                }
            }
        });
    }

    //-- read all data from database --//
    function readAllData(table) {
        if(!db) { return; }
        var value = []
        db.transaction( function(tx) {
            var result = tx.executeSql('SELECT * from '+ table);
            for (var i=0; i<result.rows.length; i++){
                value.push(result.rows.item(i))
            }

        });

        return value
    }


    //-- read from database --//
    function readData(table, id) {
        if(!db) { return; }
        var obj
        db.transaction( function(tx) {
            var result = tx.executeSql('select * from '+ table +' where id=(?)', id);
            if(result.rows.length === 1) {
                // get the value column
                obj = result.rows.item(0)

            }
        });
        return obj
    }

    function generateRandom(array) {
        var max = Math.max(...array)
        var num = max + 1
        return num;
    }
}
