import { GameState, GameAction } from 'damath-zero_pb';
import { DamathZeroClient } from 'damath-zero_grpcweb_pb';

const client = new DamathZeroClient('http://localhost:50051');

const gameState = new GameState();
gameState.setName('test');

client.predict(gameState, {}, (err, response) => {
    if (err) {
        console.error(err);
    } else {
        console.log(response);
    }
});