select
trim(ref_region.description) as description,
trim(sales.network) as network,
sum(amount) as total_sales,
count(amount) as transactions
from sales left join ref_region on sales.fk_region_id = ref_region.id
group by ref_region.description,sales.network
order by ref_region.description,sales.network;
