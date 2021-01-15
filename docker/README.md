docker reset:

    docker system prune -a


docker build interactive:

	docker build --tag vtestnet:eosio-inter \
	    --build-arg USER_ID=$(id -u) \
	    --build-arg GROUP_ID=$(id -g) docker/vtestnet-eosio.interactive


docker run interactive:

	docker run -it --mount type=bind,source="$(pwd)",target=/vapaee-smart-contracts vtestnet:eosio-inter bash
		
