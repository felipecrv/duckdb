#include "duckdb/catalog/catalog_entry/view_catalog_entry.hpp"

#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/parser/parsed_data/alter_table_info.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "duckdb/common/limits.hpp"

#include <algorithm>

namespace duckdb {

void ViewCatalogEntry::Initialize(CreateViewInfo &info) {
	query = std::move(info.query);
	this->aliases = info.aliases;
	this->types = info.types;
	this->temporary = info.temporary;
	this->sql = info.sql;
	this->internal = info.internal;
	this->comment = info.comment;
}

ViewCatalogEntry::ViewCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateViewInfo &info)
    : StandardEntry(CatalogType::VIEW_ENTRY, schema, catalog, info.view_name) {
	Initialize(info);
}

unique_ptr<CreateInfo> ViewCatalogEntry::GetInfo() const {
	auto result = make_uniq<CreateViewInfo>();
	result->schema = schema.name;
	result->view_name = name;
	result->sql = sql;
	result->query = unique_ptr_cast<SQLStatement, SelectStatement>(query->Copy());
	result->aliases = aliases;
	result->types = types;
	result->temporary = temporary;
	result->comment = comment;
	return std::move(result);
}

unique_ptr<CatalogEntry> ViewCatalogEntry::AlterEntry(ClientContext &context, AlterInfo &info) {
	D_ASSERT(!internal);
	if (info.type != AlterType::ALTER_VIEW) {
		throw CatalogException("Can only modify view with ALTER VIEW statement");
	}
	auto &view_info = info.Cast<AlterViewInfo>();
	switch (view_info.alter_view_type) {
	case AlterViewType::RENAME_VIEW: {
		auto &rename_info = view_info.Cast<RenameViewInfo>();
		auto copied_view = Copy(context);
		copied_view->name = rename_info.new_view_name;
		return copied_view;
	}
	default:
		throw InternalException("Unrecognized alter view type!");
	}
}

string ViewCatalogEntry::ToSQL() const {
	if (sql.empty()) {
		//! Return empty sql with view name so pragma view_tables don't complain
		return sql;
	}
	auto info = GetInfo();
	auto result = info->ToString();
	return result + ";\n";
}

unique_ptr<CatalogEntry> ViewCatalogEntry::Copy(ClientContext &context) const {
	D_ASSERT(!internal);
	auto create_info = GetInfo();

	return make_uniq<ViewCatalogEntry>(catalog, schema, create_info->Cast<CreateViewInfo>());
}

} // namespace duckdb
