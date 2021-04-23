update raw_sales
set fk_region_id = ref_region.id
from ref_region
where
raw_sales.region = ref_region.region and
raw_sales.trans_date >= ref_region.start_date and
raw_sales.trans_date <= ref_region.end_date;

insert into sales ( identifier,network,region,trans_date,product,amount,fk_region_id)
select identifier,network,region,trans_date,product,amount,fk_region_id from raw_sales;

drop table if exists raw_sales;