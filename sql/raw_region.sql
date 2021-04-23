drop table if exists raw_ref_region;
create table raw_ref_region
(
  region char(20) not null,
  description char(100) not null,
  start_date date not null,
  end_date date
);

COPY raw_ref_region(region, description, start_date, end_date) 
FROM '%filename%' 
DELIMITER ',' 
QUOTE ''''
CSV HEADER;