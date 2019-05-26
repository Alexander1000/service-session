# Changelog service-session

## 2019-05-26
### Changed
  - listen host port add to config
  - generate method for create
  - implement method create session

## 2019-05-25
### Changed
  - Добавлена обработка флагов
  - Конфигурирование из json-файла

## 2019-04-15
### Changed
  - `tnt_insert` exchange on `tnt_replace` for save tuple

## 2019-04-14
### Added
  - add save request
  - rename `getBySessId` -> `getById` in storage
  - implement save tuple to tarantool

## 2019-05-12
### Added
  - add storage class
  - integration with tarantool storage (unpack result)
  - return matched data from tarantool to response

## 2019-05-09
### Changed
  - include tarantool library

## 2019-04-30
### Changed
  - use cmake for build project

## 2019-04-20
### Add
  - Add protobuf and generate message
  - Generater grpc server
  - add entrypoint
  - add makefile

## 2019-04-16
### Add
  - Initial commit
