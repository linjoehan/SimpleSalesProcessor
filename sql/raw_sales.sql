drop table if exists raw_sales;
create table raw_sales
(
  identifier char(20) not null,
  network char(20) not null,
  region char(20) not null,
  trans_date date not null,
  product char(20) not null,
  amount decimal(15,2) not null,
  fk_region_id bigint
);

COPY raw_sales(identifier, network, region, trans_date , product , amount) 
FROM '%filename%'
DELIMITER ',' 
QUOTE ''''
CSV HEADER;