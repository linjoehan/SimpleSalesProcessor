drop database if exists simplesalesprocessor;
create database simplesalesprocessor;

\connect simplesalesprocessor

drop table if exists ref_region cascade;
create table ref_region
(
  id serial not null,
  region char(20) not null,
  description char(100) not null,
  start_date date not null,
  end_date date not null,
  primary key(id),
  constraint uq_ref_region_region_start_date unique (region,start_date)
);

drop table if exists sales cascade;
create table sales
(
  record_id bigserial primary key not null,
  identifier char(20) not null,
  network char(20) not null,
  region char(20) not null,
  trans_date date not null,
  product char(20) not null,
  amount decimal(15,2) not null,
  fk_region_id bigint references ref_region(id)
);

