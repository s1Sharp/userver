## MongoDB service 


## Before you start

Make sure that you can compile and run core tests and read a basic example
@ref md_en_userver_tutorial_hello_service.


## Step by step guide

In this tutorial we will write a service that stores history of translation
changes and returns the most recent translations. MongoDB would be used as a
database. The service would have the following Rest API:

* HTTP PATCH by path '/v1/translations' with query parameters
  'key', 'lang' and 'value' updates a translation.
* HTTP POST by path '/v1/translations' with query parameter
  'last_update' returns unique translations that were added after the 'last_update'.


### HTTP handler component

Like in @ref md_en_userver_tutorial_hello_service we create a component for
handling HTTP requests:

@snippet samples/mongo_service.cpp  Mongo service sample - component

Note that the component holds a storages::mongo::PoolPtr - a client to the Mongo.
That client is thread safe, you can use it concurrently from different threads
and tasks.


### Translations::InsertNew

In the `Translations::InsertNew` function we get the request arguments and form
a BSON document for insertion.

@snippet samples/mongo_service.cpp  Mongo service sample - InsertNew

There are different ways to form a document, see @ref md_en_userver_formats.


### Translations::ReturnDiff

MongoDB queries are just BSON documents. Each mongo document has an implicit
`_id` field that stores the document creation time. Knowing that, we can use
formats::bson::Oid::MakeMinimalFor() to find all the documents that were added
after `update_time`. Query sorts the documents by modification times (by `_id`),
so when the results are written into formats::json::ValueBuilder latter writes
rewrite previous data for the same key.

@snippet samples/mongo_service.cpp  Mongo service sample - ReturnDiff

See @ref md_en_userver_mongodb for MongoDB hints and more usage samples.


### Static config

Static configuration of service is quite close to the configuration from
@ref md_en_userver_tutorial_hello_service except for the handler and DB:

@snippet samples/mongo_service.cpp  Mongo service sample - static config

There are more static options for the MongoDB component configuration, all of
them are described at components::Mongo.


### Dynamic config

We are not planning to get new dynamic config values in this sample. Because of
that we just write the defaults to the fallback file of the `components::TaxiConfigFallbacksComponent` component.

All the values are described in a separate section @ref md_en_schemas_dynamic_configs .

@snippet samples/mongo_service.cpp  Mongo service sample - dynamic config

A production ready service would dynamically retrieve the above options at runtime from a configuration service. See
@ref md_en_userver_tutorial_config_service for insights on how to change the
above options on the fly, without restarting the service.


### int main()

Finally, after writing down the dynamic config values into file at 
`taxi-config-fallbacks.fallback-path`, we add our component to the
components::MinimalServerComponentList(),
and start the server with static configuration `kStaticConfig`.

@snippet samples/mongo_service.cpp  Mongo service sample - main


### Build

To build the sample, execute the following build steps at the userver root directory:
```
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release ..
make userver-samples-mongo_service
```

Start the DB server and then start the service by running `./samples/userver-samples-mongo_service`.
Now you can send a request to your service from another terminal:
```
bash
$ curl -X PATCH 'http://localhost:8090/v1/translations?key=hello&lang=ru&value=Привки'
$ curl -X PATCH 'http://localhost:8090/v1/translations?key=hello&lang=ru&value=Дратути'
$ curl -X PATCH 'http://localhost:8090/v1/translations?key=hello&lang=ru&value=Здрасьте'
$ curl -s -X POST http://localhost:8090/v1/translations?last_update=2021-11-01T12:00:00Z | jq
{
  "content": {
    "hello": {
      "ru": "Дратути"
    },
    "wellcome": {
      "ru": "Здрасьте"
    }
  },
  "update_time": "2021-12-20T10:17:37.249767773+00:00"
}
```

## Full sources

See the full example at @ref samples/mongo_service.cpp
@example samples/mongo_service.cpp